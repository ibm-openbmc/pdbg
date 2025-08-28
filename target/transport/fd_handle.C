#include "fd_handle.H"

#include <utility> // for std::exchange

namespace transport
{

Fd::Fd(int f) noexcept : fd(f) {}

Fd::~Fd() noexcept
{
    if (fd >= 0)
    {
        ::close(fd);
    }
}

Fd::Fd(Fd&& other) noexcept : fd(std::exchange(other.fd, -1)) {}

int Fd::get() const noexcept
{
    return fd;
}

Fd::operator bool() const noexcept
{
    return fd >= 0;
}

} // namespace transport
