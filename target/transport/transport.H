#pragma once

#include <sys/ioctl.h>
#include <targeting/target.H>

#include <cstdint>

namespace transport
{
using ByteVector = std::vector<std::byte>;
constexpr std::uint32_t MAX_SBE_RESP_SIZE = 0x4096;

int getCfam(TARGETING::ConstTargetPtr target, std::uint32_t addr,
            std::uint32_t& value);

int putCfam(TARGETING::ConstTargetPtr target, std::uint32_t addr,
            std::uint32_t data);

int getScom(TARGETING::ConstTargetPtr target, std::uint64_t addr,
            std::uint64_t& value);

int putScom(TARGETING::ConstTargetPtr target, std::uint64_t addr,
            std::uint64_t value);

/**
 * @brief Enum for SBEFIFO ioctl commands
 */
enum class SBEIoctl : std::uint32_t
{
    SbefifoReadTimeout = _IOW('s', 0x00, unsigned int),
    // Future ioctls can be added here
};

int sendAndRecv(TARGETING::ConstTargetPtr target,
                std::span<const std::byte> cmd, int timeout_ms,
                ByteVector& out);
} // namespace transport
