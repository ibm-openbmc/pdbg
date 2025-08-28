#pragma once

#include <endian.h> // beXXtoh / htobeXX
#include <sys/ioctl.h>
#include <targeting/target.H>

#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

namespace transport
{
using ByteVector = std::vector<std::byte>;
constexpr std::uint32_t MAX_SBE_RESP_SIZE = 0x9999;
namespace direct
{
int getCfam(TARGETING::ConstTargetPtr target, std::uint32_t addr64,
            std::uint32_t& value);

int putCfam(TARGETING::ConstTargetPtr target, std::uint32_t addr64,
            std::uint32_t data);

int getScom(TARGETING::ConstTargetPtr target, std::uint64_t addr,
            std::uint64_t& value);

int putScom(TARGETING::ConstTargetPtr target, std::uint64_t addr,
            std::uint64_t value);
} // namespace direct
namespace sbefifo
{
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
} // namespace sbefifo
} // namespace transport
