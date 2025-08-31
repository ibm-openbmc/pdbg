#pragma once

#include <target.H>
#include <attributeenums.H>
#include <attributetraits.H>

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
    int getCfam(TargetPtr_t target, uint32_t addr, uint32_t& val);
    int putCfam(TargetPtr_t target, uint32_t addr, uint32_t val);
    virtual int getScom(TargetPtr_t target, uint64_t addr, uint64_t& val){return -1;}
    virtual int putScom(TargetPtr_t target, uint64_t addr, uint64_t val){return -1;}
};
}  // namespace hwaccess
