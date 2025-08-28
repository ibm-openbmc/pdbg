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
namespace direct
{
int getCfam(TARGETING::ConstTargetPtr target, std::uint32_t addr64,
            std::uint32_t& value);
} // namespace direct
} // namespace transport
