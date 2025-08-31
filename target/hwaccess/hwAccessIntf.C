#include <hwAccessIntf.H>
#include "hwBaseAccess.H"
#include "hwDirectAccess.H"
#include "hwSbeAccess.H"

namespace hwaccess
{
using HwAccessPtr = TARGETING::AttributeTraits<TARGETING::ATTR_HW_ACCESS_PTR>::Type;

int HwAccessIntf::getCfamRegister(ConstTargetPtr_t target, uint32_t addr, uint32_t& val)
{
    if(target == nullptr)
    {
        std::cerr << "getCfamRegister: target is nullptr" << std::endl;
        return -1;
    }

    auto accessPtr = target->getAttr<TARGETING::ATTR_HW_ACCESS_PTR>();
    HwBaseAccess* hwAccessPtr = reinterpret_cast<HwBaseAccess*>(static_cast<HwAccessPtr>(accessPtr));

    if (hwAccessPtr == nullptr)
    {
        std::cerr << "getCfamRegister: hwAccessPtr is nullptr" << std::endl;
        return -1;
    }

    return hwAccessPtr->getCfam(target, addr, val);
}

int HwAccessIntf::putCfamRegister(ConstTargetPtr_t target, uint32_t addr, uint32_t val)
{
    if(target == nullptr)
    {
        std::cerr << "putCfamRegister: target is nullptr" << std::endl;
        return -1;
    }

    auto accessPtr = target->getAttr<TARGETING::ATTR_HW_ACCESS_PTR>();
    HwBaseAccess* hwAccessPtr = reinterpret_cast<HwBaseAccess*>(static_cast<HwAccessPtr>(accessPtr));

    if (hwAccessPtr == nullptr)
    {
        std::cerr << "putCfamRegister: hwAccessPtr is nullptr" << std::endl;
        return -1;
    }

    return hwAccessPtr->putCfam(target, addr, val);
}

int HwAccessIntf::getScomRegister(ConstTargetPtr_t target, uint64_t addr, uint64_t& val)
{
    if(target == nullptr)
    {
        std::cerr << "getScomRegister: target is nullptr" << std::endl;
        return -1;
    }

    auto accessPtr = target->getAttr<TARGETING::ATTR_HW_ACCESS_PTR>();
    HwBaseAccess* hwAccessPtr = reinterpret_cast<HwBaseAccess*>(static_cast<HwAccessPtr>(accessPtr));

    if (hwAccessPtr == nullptr)
    {
        std::cerr << "getScomRegister: hwAccessPtr is nullptr" << std::endl;
        return -1;
    }

    return hwAccessPtr->getScom(target, addr, val);
}

int HwAccessIntf::putScomRegister(ConstTargetPtr_t target, uint64_t addr, uint64_t val)
{
    if(target == nullptr)
    {
        std::cerr << "putScomRegister: target is nullptr" << std::endl;
        return -1;
    }

    auto accessPtr = target->getAttr<TARGETING::ATTR_HW_ACCESS_PTR>();
    HwBaseAccess* hwAccessPtr = reinterpret_cast<HwBaseAccess*>(static_cast<HwAccessPtr>(accessPtr));

    if (hwAccessPtr == nullptr)
    {
        std::cerr << "putScomRegister: hwAccessPtr is nullptr" << std::endl;
        return -1;
    }

    return hwAccessPtr->putScom(target, addr, val);
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
    std::cerr << "getHwAccessPtr: HwAccessMethod is invalid" << std::endl;
    return nullptr;
}
} //namespace hwaccess
