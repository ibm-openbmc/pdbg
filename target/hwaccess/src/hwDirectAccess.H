#pragma once

#include "hwBaseAccess.H"

namespace HWACCESS
{
class HwDirectAccess : public HwBaseAccess
{
protected:
    HwDirectAccess() = default;
    HwDirectAccess(const HwDirectAccess&) = delete;
    HwDirectAccess& operator=(const HwDirectAccess&) = delete;
public:
    static HwDirectAccess& getInstance();
    virtual int getScom(TargetPtr_t target, uint64_t addr, uint64_t& val) override final;
    virtual int putScom(TargetPtr_t target, uint64_t addr, uint64_t val) override final;
};
} //namespace HWACCESS
