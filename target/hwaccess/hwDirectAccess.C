#include "hwDirectAccess.H"

#include <transport.H>

namespace hwaccess
{
HwDirectAccess& HwDirectAccess::getInstance()
{
    static HwDirectAccess directAccessInstance;
    return directAccessInstance;
}

int HwDirectAccess::getScom(ConstTargetPtr_t target, uint64_t addr, uint64_t& val)
{
    if(target == nullptr)
    {
        std::cerr << "HwDirectAccess::getScom target is nullptr" << std::endl;
        return -1;
    }

    std::cout << "HwDirectAccess::getScom executed" << std::endl;
    return transport::direct::getScom(target, addr, val);
}

int HwDirectAccess::putScom(ConstTargetPtr_t target, uint64_t addr, uint64_t val)
{
    if(target == nullptr)
    {
        std::cerr << "HwDirectAccess::putScom target is nullptr" << std::endl;
        return -1;
    }

    std::cout << "HwDirectAccess::putScom executed" << std::endl;
    return transport::direct::putScom(target, addr, val);
}
} // namespace hwaccess
