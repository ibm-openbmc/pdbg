#include "fd_handle.H"
#include "transport.H"

#include <fcntl.h>
#include <sys/ioctl.h>
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

std::optional<std::string> getFSIBasePath()
{
    if (::access(OPENFSI_PATH.data(), F_OK) == 0)
    {
        std::cerr << "getFSIBasePath Found FSI base path: " << OPENFSI_PATH
                  << "\n";
        return std::string(OPENFSI_PATH);
    }
    if (::access(OPENFSI_LEGACY_PATH.data(), F_OK) == 0)
    {
        std::cerr << "getFSIBasePath Found legacy FSI base path: "
                  << OPENFSI_LEGACY_PATH << "\n";
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
    std::cerr << "fsiScanDevices Attempting to open: " << path << "\n";

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

    std::cerr << "fsiScanDevices Successfully triggered FSI rescan\n";
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

    const auto full = std::filesystem::path(*base) / fsiPath;

    static bool first_probe = true;
    constexpr int tries_max = 5;

    for (int tries = tries_max; tries > 0; --tries)
    {
        if (FdHandle fd{::open(full.c_str(), O_RDWR | O_SYNC)}; fd)
        {
            first_probe = false;
            return fd;
        }

        std::cerr << "fsiProbe open failed on " << full.string()
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

    std::cerr << "fsiProbe unable to open FSI node " << full.string()
              << " after retries errno=" << errno << " (" << strerror(errno)
              << ")\n";
    return std::nullopt;
}

int recv_all(int fd, ByteVector& out)
{
    out.resize(MAX_SBE_RESP_SIZE);
    // ssize_t n = ::read(fd, out.data(), out.size());
    ssize_t n = ::read(fd, out.data(), 4096);
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

namespace direct
{
int getCfam(TARGETING::ConstTargetPtr target, std::uint32_t addr,
            std::uint32_t& value)
{
    const auto off = encodeFsiAddr(addr);
    auto fdOpt = fsiProbe(target);
    if (!fdOpt)
    {
        std::cerr << "getCfam failed for addr=0x" << std::hex << addr << "\n";
        return -1;
    }

    FdHandle& fd = *fdOpt;
    if (::lseek(fd.get(), off, SEEK_SET) < 0)
    {
        std::cerr << "getCfam lseek failed addr=0x" << std::hex << addr
                  << " errno=" << errno << " (" << strerror(errno) << ")\n";
        return -1;
    }

    if (::read(fd.get(), &value, sizeof(value)) < 0)
    {
        std::cerr << "getCfam read failed addr=0x" << std::hex << addr
                  << " errno=" << errno << " (" << strerror(errno) << ")\n";
        return -1;
    }

    value = be32toh(value);
    std::cout << "getcfam for addr=0x" << std::hex << addr << " value=0x"
              << value << "\n";
    return 0;
}

int getScom(TARGETING::ConstTargetPtr target, std::uint64_t addr,
            std::uint64_t& value)
{
    TARGETING::ATTR_DIRECT_ACCESS_DEVICE_PATH_typeStdArr dev{};
    if (!target->tryGetAttr<TARGETING::ATTR_DIRECT_ACCESS_DEVICE_PATH>(dev))
    {
        std::cerr
            << "getScom missing ATTR_DIRECT_ACCESS_DEVICE_PATH for target\n";
        return -1;
    }

    FdHandle fd{::open(dev.data(), O_RDONLY | O_SYNC)};
    if (!fd)
    {
        std::cerr << "getScom open failed path=" << dev.data()
                  << " errno=" << errno << " (" << strerror(errno) << ")\n";
        return -1;
    }

    if (::pread(fd.get(), &value, sizeof(value), addr) < 0)
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

} // namespace direct

namespace sbefifo
{
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
} // namespace sbefifo
} // namespace transport
