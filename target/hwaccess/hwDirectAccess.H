#pragma once

#include "hwBaseAccess.H"

namespace hwaccess
{
class HwDirectAccess : public HwBaseAccess
{
protected:
    HwDirectAccess() = default;
    HwDirectAccess(const HwDirectAccess&) = delete;
    HwDirectAccess& operator=(const HwDirectAccess&) = delete;
public:
    static HwDirectAccess& getInstance();
    virtual int getScom(ConstTargetPtr_t target, uint64_t addr, uint64_t& val) override final;
    virtual int putScom(ConstTargetPtr_t target, uint64_t addr, uint64_t val) override final;
};
} //namespace hwaccess
