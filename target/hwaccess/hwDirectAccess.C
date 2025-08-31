#include "hwDirectAccess.H"

#include <transport.H>

namespace hwaccess
{
HwDirectAccess& HwDirectAccess::getInstance()
{
    static HwDirectAccess directAccessInstance;
    return directAccessInstance;
}

int HwDirectAccess::getScom(TargetPtr_t target, uint64_t addr, uint64_t& val)
{
    if(target == nullptr)
    {
        return -1;
    }

    std::cout << "HwDirectAccess::getScom executed" << std::endl;
    return transport::direct::getScom(target, addr, val);
}

int HwDirectAccess::putScom(TargetPtr_t target, uint64_t addr, uint64_t val)
{
    if(target == nullptr)
    {
        return -1;
    }

    std::cout << "HwDirectAccess::putScom executed" << std::endl;
    return transport::direct::putScom(target, addr, val);
}
} // namespace hwaccess
