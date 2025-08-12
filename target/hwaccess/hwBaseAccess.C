#include "hwBaseAccess.H"
#include "hwTransport.H"

namespace HWACCESS
{
int HwBaseAccess::getCfam(TargetPtr_t target, uint32_t addr, uint32_t& val)
{
    if(target == nullptr)
    {
        return -1;
    }

    std::cout << "HwBaseAccess::getCfam executed" << std::endl;
    return HWTRANSPORT::HwTransportIntf::getCfam(target, addr, val);
}

int HwBaseAccess::putCfam(TargetPtr_t target, uint32_t addr, uint32_t val)
{
    if(target == nullptr)
    {
        return -1;
    }
    
    std::cout << "HwBaseAccess::putCfam executed" << std::endl;
    return HWTRANSPORT::HwTransportIntf::putCfam(target, addr, val);
}
} //namespace HWACCESS
