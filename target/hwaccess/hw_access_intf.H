#pragma once

#include <targeting/target.H>
#include <targeting/xmltohb/attributeenums.H>
#include <targeting/xmltohb/attributetraits.H>

namespace hwaccess
{
using HwAccessMethod = TARGETING::AttributeTraits<TARGETING::ATTR_HW_ACCESS_METHOD>::Type;
using ConstTargetPtr_t = TARGETING::ConstTargetPtr;

/**
*
* @note User of these library methods is expected to handle exceptions

* Exception will be thrown when Register operations are done without setting HwAccessMethod
* setHwAccessMethod is provided by target service.
*
**/
class HwAccessIntf
{
    public:
    static int getCfamRegister(ConstTargetPtr_t target, uint32_t addr, uint32_t& val);
    static int putCfamRegister(ConstTargetPtr_t target, uint32_t addr, uint32_t val);
    static int getScomRegister(ConstTargetPtr_t target, uint64_t addr, uint64_t& val);
    static int putScomRegister(ConstTargetPtr_t target, uint64_t addr, uint64_t val);
    static void* getHwAccessPtr(HwAccessMethod& accessMethod);
};
} //namespace hwaccess
