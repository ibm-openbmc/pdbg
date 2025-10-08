#include "fd_handle.H"
#include "transport.H"

#include <fcntl.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <filesystem>
#include <format>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

namespace transport
{

namespace // unnamed namespace
{
constexpr std::string_view OPENFSI_PATH = "/sys/class/fsi-master/";
constexpr std::string_view OPENFSI_LEGACY_PATH =
    "/sys/bus/platform/devices/gpio-fsi/";

std::uint32_t encodeFsiAddr(std::uint32_t addr)
{
    return (addr & 0x7ffc00u) | ((addr & 0x3ffu) << 2);
}

std::string getPhyPath(TARGETING::ConstTargetPtr tgt)
{
    if (!tgt)
    {
        return "";
    }

    TARGETING::EntityPath path;
    if (tgt->tryGetAttr<TARGETING::ATTR_PHYS_PATH>(path))
    {
        return path.toString();
    }
    return "";
}

std::optional<std::string> getFSIBasePath()
{
    if (::access(OPENFSI_PATH.data(), F_OK) == 0)
    {
        return std::string(OPENFSI_PATH);
    }
    if (::access(OPENFSI_LEGACY_PATH.data(), F_OK) == 0)
    {
        return std::string(OPENFSI_LEGACY_PATH);
    }

    std::cerr << "getFSIBasePath No valid FSI base path found\n";
    return std::nullopt;
}

int fsiScanDevices()
{
    const auto base = getFSIBasePath();
    if (!base)
    {
        std::cerr << "fsiScanDevices: No FSI path available\n";
        return -1;
    }

    const auto path = std::filesystem::path(*base) / "fsi0" / "rescan";
    FdHandle fd{::open(path.c_str(), O_WRONLY | O_SYNC)};
    if (!fd)
    {
        std::cerr << "fsiScanDevices Failed to open " << path << ": "
                  << std::strerror(errno) << "\n";
        return -1;
    }

    const char one = '1';
    if (::write(fd.get(), &one, 1) < 0)
    {
        std::cerr << "fsiScanDevices Failed to write to " << path << ": "
                  << std::strerror(errno) << "\n";
        return -1;
    }
    return 0;
}

std::optional<FdHandle> fsiProbe(TARGETING::ConstTargetPtr target)
{
    TARGETING::ATTR_FSI_DEVICE_PATH_typeStdArr fsiAttr{};
    if (!target->tryGetAttr<TARGETING::ATTR_FSI_DEVICE_PATH>(fsiAttr))
    {
        std::cerr << "fsiProbe missing ATTR_FSI_DEVICE_PATH for target\n";
        return std::nullopt;
    }

    const auto base = getFSIBasePath();
    if (!base)
    {
        std::cerr << "fsiProbe FSI base path not found for target\n";
        return std::nullopt;
    }

    std::string fsiPath(fsiAttr.data());

    // ensure it’s relative
    if (!fsiPath.empty() && fsiPath.front() == '/')
    {
        fsiPath.erase(0, 1);
    }

    const auto fullPath = std::filesystem::path(*base) / fsiPath;
    std::cout << "transport fsiProbe fsifullpath " << fullPath
              << " target path " << getPhyPath(target) << "\n";
    static bool first_probe = true;
    constexpr int tries_max = 5;

    for (int tries = tries_max; tries > 0; --tries)
    {
        if (FdHandle fd{::open(fullPath.c_str(), O_RDWR | O_SYNC)}; fd)
        {
            first_probe = false;
            return fd;
        }

        std::cerr << "fsiProbe open failed on " << fullPath.string()
                  << " errno=" << errno << " (" << strerror(errno) << ")\n";

        if (first_probe)
        {
            fsiScanDevices();
            ::sleep(1);
        }
        else
        {
            break;
        }
    }

    std::cerr << "fsiProbe unable to open FSI node " << fullPath.string()
              << " after retries errno=" << errno << " (" << strerror(errno)
              << ")\n";
    return std::nullopt;
}

std::optional<FdHandle*> prepareCfamAccess(TARGETING::ConstTargetPtr target,
                                           std::uint32_t addr)
{
    const auto off = encodeFsiAddr(addr);
    auto fdOpt = fsiProbe(target);
    if (!fdOpt)
    {
        std::cerr << "CFAM access failed for addr=0x" << std::hex << addr
                  << "\n";
        return std::nullopt;
    }

    FdHandle& fd = *fdOpt;
    if (::lseek(fd.get(), off, SEEK_SET) < 0)
    {
        std::cerr << "CFAM lseek failed addr=0x" << std::hex << addr
                  << " errno=" << errno << " (" << strerror(errno) << ")\n";
        return std::nullopt;
    }

    return &fd;
}

std::optional<FdHandle> prepareScomAccess(TARGETING::ConstTargetPtr target,
                                          int flags)
{
    TARGETING::ATTR_DIRECT_ACCESS_DEVICE_PATH_typeStdArr dev{};
    if (!target->tryGetAttr<TARGETING::ATTR_DIRECT_ACCESS_DEVICE_PATH>(dev))
    {
        std::cerr << "prepareScomAccess : "
                  << " missing ATTR_DIRECT_ACCESS_DEVICE_PATH for target\n";
        return std::nullopt;
    }

    FdHandle fd{::open(dev.data(), flags | O_SYNC)};
    if (!fd)
    {
        std::cerr << "prepareScomAccess open failed path=" << dev.data()
                  << " errno=" << errno << " (" << strerror(errno) << ")\n";
        return std::nullopt;
    }

    return fd;
}

int recv_all(int fd, ByteVector& out)
{
    out.resize(MAX_SBE_RESP_SIZE);
    ssize_t n = ::read(fd, out.data(), out.size());
    if (n < 0)
    {
        std::cerr << "recv_all read(sbefifo) failed: " << std::strerror(errno)
                  << " (errno=" << errno << ")\n";
        return -1;
    }

    out.resize(static_cast<std::size_t>(n));
    return 0;
}

int send_all(int fd, std::span<const std::byte> cmd)
{
    std::size_t total = 0;
    while (total < cmd.size())
    {
        ssize_t n = ::write(fd, cmd.data() + total, cmd.size() - total);
        if (n < 0)
        {
            std::cerr << "send_all write(sbefifo) failed: "
                      << std::strerror(errno) << " (errno=" << errno
                      << ", written=" << total
                      << ", remaining=" << (cmd.size() - total) << ")\n";
            return -1;
        }
        total += static_cast<std::size_t>(n);
    }

    return 0;
}

} // unnamed namespace

int getCfam(TARGETING::ConstTargetPtr target, std::uint32_t addr,
            std::uint32_t& value)
{

    auto fdPtrOpt = prepareCfamAccess(target, addr);
    if (!fdPtrOpt)
    {
        return -1;
    }

    FdHandle* fd = *fdPtrOpt;
    if (::read(fd->get(), &value, sizeof(value)) < 0)
    {
        std::cerr << "getCfam read failed addr=0x" << std::hex << addr
                  << " errno=" << errno << " (" << strerror(errno) << ")\n";
        return -1;
    }

    value = be32toh(value);
    std::cout << "transport: getcfam for addr=0x" << std::hex << addr << " value=0x"
              << value << "\n";
    return 0;
}

int putCfam(TARGETING::ConstTargetPtr target, std::uint32_t addr,
            std::uint32_t value)
{
    auto fdPtrOpt = prepareCfamAccess(target, addr);
    if (!fdPtrOpt)
    {
        return -1;
    }

    FdHandle* fd = *fdPtrOpt;
    value = htobe32(value);
    if (::write(fd->get(), &value, sizeof(value)) < 0)
    {
        std::cerr << "putCfam write failed addr=0x" << std::hex << addr << "\n";
        return -1;
    }

    std::cout << "transport putcfam addr=0x" << std::hex << addr << " value=0x"
              << value << "\n";
    return 0;
}

int getScom(TARGETING::ConstTargetPtr target, std::uint64_t addr,
            std::uint64_t& value)
{
    auto fdOpt = prepareScomAccess(target, O_RDONLY);
    if (!fdOpt)
    {
        std::cerr << "getScom failed to open fd "
                  << " errno=" << errno << " (" << strerror(errno) << ")\n";
        return -1;
    }

    FdHandle& fd = *fdOpt;
    if (::pread(fd.get(), &value, sizeof(value), addr) != sizeof(value))
    {
        std::cerr << "getScom pread failed addr=0x" << std::hex << addr
                  << " errno=" << errno << " (" << strerror(errno) << ")\n";
        return -1;
    }

    value = be64toh(value);
    std::cout << "getScom for addr=0x" << std::hex << addr << " value=0x"
              << value << "\n";
    return 0;
}

int putScom(TARGETING::ConstTargetPtr target, std::uint64_t addr,
            std::uint64_t value)
{
    auto fdOpt = prepareScomAccess(target, O_RDWR);
    if (!fdOpt)
    {
        std::cerr << "putScom failed to open fd"
                  << " errno=" << errno << " (" << strerror(errno) << ")\n";
        return -1;
    }

    FdHandle& fd = *fdOpt;
    uint64_t tmp = htobe64(value);
    if (::pwrite(fd.get(), &tmp, sizeof(tmp), addr) != sizeof(tmp))
    {
        std::cerr << "putScom pwrite failed addr=0x" << std::hex << addr
                  << " errno=" << errno << " (" << strerror(errno) << ")\n";
        return -1;
    }
    std::cout << "putScom for addr=0x" << std::hex << addr << " value=0x"
              << value << "\n";
    return 0;
}

int sendAndRecv(TARGETING::ConstTargetPtr target,
                std::span<const std::byte> cmd, int timeout_ms, ByteVector& out)
{
    int rc = 0;

    TARGETING::ATTR_SBEFIFO_DEVICE_PATH_typeStdArr fifoPath{};
    if (!target->tryGetAttr<TARGETING::ATTR_SBEFIFO_DEVICE_PATH>(fifoPath))
    {
        std::cerr
            << "sendAndRecv missing ATTR_SBEFIFO_DEVICE_PATH for target\n";
        return -1;
    }

    FdHandle fd{::open(fifoPath.data(), O_RDWR | O_SYNC)};
    if (!fd)
    {
        std::cerr << "sendAndRecv open failed " << fifoPath.data()
                  << " errno=" << errno << " (" << strerror(errno) << ")\n";
        return -1;
    }

    rc = ::ioctl(fd.get(),
                 static_cast<unsigned long>(SBEIoctl::SbefifoReadTimeout),
                 &timeout_ms);
    if (rc != 0)
    {
        std::cerr << "sendAndRecv ioctl(SbefifoReadTimeout) failed rc=" << rc
                  << " errno=" << errno << " (" << strerror(errno) << ")\n";
        return rc;
    }

    rc = send_all(fd.get(), cmd);
    if (rc != 0)
    {
        std::cerr << "sendAndRecv send_all failed rc=" << rc << '\n';
        return rc;
    }

    rc = recv_all(fd.get(), out);
    if (rc != 0)
    {
        std::cerr << "sendAndRecv recv_all failed rc=" << rc << "\n";
        return rc;
    }
    return rc;
}
} // namespace transport
