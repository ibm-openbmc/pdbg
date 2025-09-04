#pragma once

#include <sys/ioctl.h>
#include <targeting/target.H>

#include <cstdint>

namespace transport
{

/**
 * @brief Byte buffer type used for SBE commands and responses.
 */
using ByteVector = std::vector<std::byte>;

/// @brief Maximum size of an SBE response buffer (16 KB).
constexpr std::uint32_t MAX_SBE_RESP_SIZE = 0x4096;

/**
 * @brief Perform a CFAM read operation.
 *
 * @param[in]  target  Target device pointer.
 * @param[in]  addr  CFAM register address (32-bit).
 * @param[out] value   Reference to store the read value.
 *
 * @retval 0   On success.
 * @retval <0  On error (errno-style).
 */
int getCfam(TARGETING::ConstTargetPtr target, std::uint32_t addr,
            std::uint32_t& value);

/**
 * @brief Perform a CFAM write operation.
 *
 * @param[in] target  Target device pointer.
 * @param[in] addr  CFAM register address (32-bit).
 * @param[in] data    Value to write.
 *
 * @retval 0   On success.
 * @retval <0  On error (errno-style).
 */
int putCfam(TARGETING::ConstTargetPtr target, std::uint32_t addr,
            std::uint32_t data);

/**
 * @brief Perform a SCOM read operation.
 *
 * @param[in]  target  Target device pointer.
 * @param[in]  addr    SCOM register address (64-bit).
 * @param[out] value   Reference to store the read value.
 *
 * @retval 0   On success.
 * @retval <0  On error (errno-style).
 */
int getScom(TARGETING::ConstTargetPtr target, std::uint64_t addr,
            std::uint64_t& value);

/**
 * @brief Perform a SCOM write operation.
 *
 * @param[in] target  Target device pointer.
 * @param[in] addr    SCOM register address (64-bit).
 * @param[in] value   Value to write.
 *
 * @retval 0   On success.
 * @retval <0  On error (errno-style).
 */
int putScom(TARGETING::ConstTargetPtr target, std::uint64_t addr,
            std::uint64_t value);

/**
 * @brief Enum for SBEFIFO ioctl commands.
 *
 * These commands are used with the `ioctl()` system call
 * to control the SBEFIFO device.
 */
enum class SBEIoctl : std::uint32_t
{
    /// @brief Configure the read timeout in milliseconds.
    SbefifoReadTimeout = _IOW('s', 0x00, unsigned int),
    // Future ioctls can be added here
};

/**
 * @brief Send a command to the SBEFIFO device and receive a response.
 *
 * @param[in]  target      Target device pointer.
 * @param[in]  cmd         Command data to send (input buffer).
 * @param[in]  timeout_ms  Timeout for the operation in milliseconds.
 * @param[out] out         Vector to hold the response data.
 *
 * @retval 0   On success.
 * @retval <0  On error (errno-style).
 */
int sendAndRecv(TARGETING::ConstTargetPtr target,
                std::span<const std::byte> cmd, int timeout_ms,
                ByteVector& out);
} // namespace transport
