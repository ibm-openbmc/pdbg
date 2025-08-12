#include <hwAccessIntf.H>
#include "hwBaseAccess.H"
#include "hwDirectAccess.H"
#include "hwSbeAccess.H"

namespace HWACCESS
{
using HwAccessPtr = TARGETING::AttributeTraits<TARGETING::ATTR_HW_ACCESS_PTR>::Type; 

int HwAccessIntf::getCfamRegisters(TargetPtr_t target, uint32_t addr, uint32_t& val)
{
    if(target == nullptr)
        return -1;

    HwAccessPtr accessPtr;

    if(target->tryGetAttr<TARGETING::ATTR_HW_ACCESS_PTR>(accessPtr))
    {
        HwBaseAccess* hwAccessPtr = reinterpret_cast<HwBaseAccess*>(static_cast<uintptr_t>(accessPtr));
        if (hwAccessPtr != nullptr)
        {
            return hwAccessPtr->getCfam(target, addr, val);
        }
    }
    return 0;
}

int HwAccessIntf::putCfamRegisters(TargetPtr_t target, uint32_t addr, uint32_t val)
{
    if(target == nullptr)
        return -1;

    HwAccessPtr accessPtr;
 
    if(target->tryGetAttr<TARGETING::ATTR_HW_ACCESS_PTR>(accessPtr))
    {
        HwBaseAccess* hwAccessPtr = reinterpret_cast<HwBaseAccess*>(static_cast<uintptr_t>(accessPtr));
        if (hwAccessPtr != nullptr)
        {
            return hwAccessPtr->putCfam(target, addr, val);
        }
    }
    return 0;
}

int HwAccessIntf::getScomRegisters(TargetPtr_t target, uint64_t addr, uint64_t& val)
{
    if(target == nullptr)
        return -1;

    HwAccessPtr accessPtr;
 
    if(target->tryGetAttr<TARGETING::ATTR_HW_ACCESS_PTR>(accessPtr))
    {
        HwBaseAccess* hwAccessPtr = reinterpret_cast<HwBaseAccess*>(static_cast<uintptr_t>(accessPtr));
        if (hwAccessPtr != nullptr)
        {
            return hwAccessPtr->getScom(target, addr, val);
        }
    }
    return 0;
}

int HwAccessIntf::putScomRegisters(TargetPtr_t target, uint64_t addr, uint64_t val)
{
    if(target == nullptr)
        return -1;

    HwAccessPtr accessPtr;
 
    if(target->tryGetAttr<TARGETING::ATTR_HW_ACCESS_PTR>(accessPtr))
    {
        HwBaseAccess* hwAccessPtr = reinterpret_cast<HwBaseAccess*>(static_cast<uintptr_t>(accessPtr));
        if (hwAccessPtr != nullptr)
        {
            return hwAccessPtr->putScom(target, addr, val);
        }
    }
    return 0;
}

void* HwAccessIntf::getHwAccessPtr(HwAccessMethod& accessMethod)
{
    if(accessMethod == TARGETING::HW_ACCESS_METHOD_DIRECT_ACCESS)
    {
        return static_cast<void*>(&HwDirectAccess::getInstance());
    }
    else if(accessMethod == TARGETING::HW_ACCESS_METHOD_SBEFIFO)
    {
        return static_cast<void*>(&HwSbeAccess::getInstance());
    }
    return nullptr;
}
} //namespace HWACCESS
