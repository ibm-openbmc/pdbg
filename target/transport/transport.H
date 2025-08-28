#pragma once

#include <targeting/target.H>

#include <cstdint>

namespace transport
{
int getCfam(TARGETING::ConstTargetPtr target, std::uint32_t addr,
            std::uint32_t& value);
int putCfam(TARGETING::ConstTargetPtr target, std::uint32_t addr,
            std::uint32_t data);

int getScom(TARGETING::ConstTargetPtr target, std::uint64_t addr,
            std::uint64_t& value);

int putScom(TARGETING::ConstTargetPtr target, std::uint64_t addr,
            std::uint64_t value);

} // namespace transport
