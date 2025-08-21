#include "dtree_loader.H"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdexcept>
#include <string>
extern "C"
{
#include <libfdt.h>
}
namespace TARGETING
{
DeviceTreeLoader::DeviceTreeLoader(const std::string& path)
{
    _fd = open(path.c_str(), O_RDWR); // open RW since you want to modify
    if (_fd < 0)
    {
        throw std::runtime_error("Failed to open DTB");
    }
    struct stat st{};
    if (fstat(_fd, &st) != 0)
    {
        close(_fd);
        throw std::runtime_error("Failed to stat DTB");
    }

    _size = st.st_size;

    _fdt = mmap(nullptr, _size, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);
    if (_fdt == MAP_FAILED)
    {
        close(_fd);
        throw std::runtime_error("Failed to mmap DTB");
    }

    if (fdt_check_header(_fdt) != 0)
    {
        munmap(_fdt, _size);
        close(_fd);
        throw std::runtime_error("Invalid DTB header");
    }
}

DeviceTreeLoader::~DeviceTreeLoader()
{
    if (_fdt != nullptr)
    {
        munmap(_fdt, _size);
    }
    if (_fd >= 0)
    {
        close(_fd);
    }
}
} // namespace TARGETING
