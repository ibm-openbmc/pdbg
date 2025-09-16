#pragma once

#include <unistd.h>

namespace transport
{

/**
 * @class Fd
 * @brief RAII wrapper for a file descriptor.
 *
 * This class manages a POSIX file descriptor, ensuring that it is
 * properly closed when the object goes out of scope.
 *
 * The class is non-copyable but movable, allowing safe transfer
 * of ownership of the descriptor.
 */
class Fd
{
  public:
    /**
     * @brief Construct a new Fd object.
     *
     * @param[in] f File descriptor to wrap. Defaults to -1 (invalid).
     */
    explicit Fd(int f = -1) noexcept;

    /**
     * @brief Destroy the Fd object.
     *
     * Closes the managed file descriptor if it is valid.
     */
    ~Fd() noexcept;

    /// @brief Deleted copy constructor (Fd is non-copyable).
    Fd(const Fd&) = delete;

    /// @brief Deleted copy assignment operator (Fd is non-copyable).
    Fd& operator=(const Fd&) = delete;

    /**
     * @brief Move constructor.
     *
     * Transfers ownership of the file descriptor from another Fd.
     *
     * @param[in,out] other The Fd to move from. After the move,
     *            `other` will no longer own the file descriptor.
     */
    Fd(Fd&& other) noexcept;

    /// @brief Deleted move assignment (not needed).
    Fd& operator=(Fd&&) = delete;

    /**
     * @brief Get the underlying file descriptor.
     *
     * @return File descriptor value, or -1 if invalid.
     */
    [[nodiscard]] int get() const noexcept;

    /**
     * @brief Check if the Fd object holds a valid descriptor.
     *
     * @retval true  If the file descriptor is valid.
     * @retval false If the file descriptor is invalid (-1).
     */
    explicit operator bool() const noexcept;

  private:
    int fd{-1}; ///< The managed file descriptor.
};

/// @brief Alias for Fd, used as a handle type.
using FdHandle = Fd;

} // namespace transport
