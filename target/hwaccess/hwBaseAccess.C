#include "hwBaseAccess.H"

#include <transport.H>

namespace hwaccess
{
int HwBaseAccess::getCfam(TargetPtr_t target, uint32_t addr, uint32_t& val)
{
    if(target == nullptr)
    {
        return -1;
    }

    std::cout << "HwBaseAccess::getCfam executed" << std::endl;
    return transport::direct::getCfam(target, addr, val);
}

int HwBaseAccess::putCfam(TargetPtr_t target, uint32_t addr, uint32_t val)
{
    if(target == nullptr)
    {
        return -1;
    }
    
    std::cout << "HwBaseAccess::putCfam executed" << std::endl;
    return transport::direct::putCfam(target, addr, val);
}
} //namespace hwaccess
