#pragma once

#include "hw_base_access.H"

namespace hwaccess
{
class HwSbeAccess : public HwBaseAccess
{
protected:
    HwSbeAccess() = default;
    HwSbeAccess(const HwSbeAccess&) = delete;
    HwSbeAccess& operator=(const HwSbeAccess&) = delete;
public:
    static HwSbeAccess& getInstance();
    virtual int getScom(ConstTargetPtr_t target, uint64_t addr, uint64_t& val) override final;
    virtual int putScom(ConstTargetPtr_t target, uint64_t addr, uint64_t val) override final;
};
} // namespace hwaccess
