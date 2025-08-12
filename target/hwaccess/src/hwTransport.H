#pragma once

#include <hwAccessIntf.H>

namespace HWTRANSPORT
{
class HwTransportIntf
{
public:
        static int getCfam(HWACCESS::TargetPtr_t target, uint32_t addr, uint32_t& val); 
        static int putCfam(HWACCESS::TargetPtr_t target, uint32_t addr, uint32_t val);
        static int getScom(HWACCESS::TargetPtr_t target, uint64_t addr, uint64_t& val); 
        static int putScom(HWACCESS::TargetPtr_t target, uint64_t addr, uint64_t val);
};
} // namespace HWTRANSPORT
