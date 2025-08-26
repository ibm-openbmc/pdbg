#pragma once

#include <unistd.h>

namespace transport
{

class Fd
{
  public:
    explicit Fd(int f = -1) noexcept;
    ~Fd() noexcept;

    // non-copyable
    Fd(const Fd&) = delete;
    Fd& operator=(const Fd&) = delete;

    // movable
    Fd(Fd&& other) noexcept;
    Fd& operator=(Fd&&) = delete; // no reassignment needed

    [[nodiscard]] int get() const noexcept;
    explicit operator bool() const noexcept;

  private:
    int fd{-1};
};

using FdHandle = Fd;

} // namespace transport
