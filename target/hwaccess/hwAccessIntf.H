#pragma once

#include <target.H>
#include <attributeenums.H>
#include <attributetraits.H>

namespace HWACCESS
{
using HwAccessMethod = TARGETING::AttributeTraits<TARGETING::ATTR_HW_ACCESS_METHOD>::Type;
using TargetPtr_t = TARGETING::TargetPtr;

class HwAccessIntf
{
    public:
    static int getCfamRegisters(TargetPtr_t target, uint32_t addr, uint32_t& val);
    static int putCfamRegisters(TargetPtr_t target, uint32_t addr, uint32_t val);
    static int getScomRegisters(TargetPtr_t target, uint64_t addr, uint64_t& val);
    static int putScomRegisters(TargetPtr_t target, uint64_t addr, uint64_t val);
    static void* getHwAccessPtr(HwAccessMethod& accessMethod);
};
} //namespace HWACCESS
