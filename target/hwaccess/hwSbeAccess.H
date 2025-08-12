#pragma once

#include "hwBaseAccess.H"

namespace HWACCESS
{
class HwSbeAccess : public HwBaseAccess
{
protected:
    HwSbeAccess() = default;
    HwSbeAccess(const HwSbeAccess&) = delete;
    HwSbeAccess& operator=(const HwSbeAccess&) = delete;
public:
    static HwSbeAccess& getInstance();
    virtual int getScom(TargetPtr_t target, uint64_t addr, uint64_t& val) override final;
    virtual int putScom(TargetPtr_t target, uint64_t addr, uint64_t val) override final;
};
} // namespace HWACCESS
