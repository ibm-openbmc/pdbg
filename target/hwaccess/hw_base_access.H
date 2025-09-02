#pragma once

#include <targeting/target.H>
#include <targeting/xmltohb/attributeenums.H>
#include <targeting/xmltohb/attributetraits.H>
#include <hwaccess/hw_access_intf.H>

namespace hwaccess
{
class HwBaseAccess 
{
protected:
    HwBaseAccess() = default;
    HwBaseAccess(const HwBaseAccess&) = delete;
    HwBaseAccess& operator=(const HwBaseAccess&) = delete;
    virtual ~HwBaseAccess() = default;

public:
    int getCfam(ConstTargetPtr_t target, uint32_t addr, uint32_t& val);
    int putCfam(ConstTargetPtr_t target, uint32_t addr, uint32_t val);
    virtual int getScom(ConstTargetPtr_t target, uint64_t addr, uint64_t& val){return -1;}
    virtual int putScom(ConstTargetPtr_t target, uint64_t addr, uint64_t val){return -1;}
};
}  // namespace hwaccess
