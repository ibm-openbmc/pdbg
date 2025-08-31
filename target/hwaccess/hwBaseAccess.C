#include "hwBaseAccess.H"

namespace hwaccess
{
int HwBaseAccess::getCfam(ConstTargetPtr_t target, uint32_t addr, uint32_t& val)
{
    if(target == nullptr)
    {
        std::cerr << "HwBaseAccess::getCfam target is nullptr" << std::endl;
        return -1;
    }

    std::cout << "HwBaseAccess::getCfam executed" << std::endl;
    return 0; // TBD: Call to hwtransport::getcfam
}

int HwBaseAccess::putCfam(ConstTargetPtr_t target, uint32_t addr, uint32_t val)
{
    if(target == nullptr)
    {
        std::cerr << "HwBaseAccess::putCfam target is nullptr" << std::endl;
        return -1;
    }
    
    std::cout << "HwBaseAccess::putCfam executed" << std::endl;
    return 0; // TBD: Call to hwtransport::putcfam
}
} //namespace hwaccess
