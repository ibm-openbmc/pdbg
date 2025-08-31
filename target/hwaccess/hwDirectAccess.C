#include "hwDirectAccess.H"

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
        std::cerr << "HwDirectAccess::getScom target is nullptr" << std::endl;
        return -1;
    }

    std::cout << "HwDirectAccess::getScom executed" << std::endl;
    return 0; // TBD: Call to hwtransport::getScom
}

int HwDirectAccess::putScom(TargetPtr_t target, uint64_t addr, uint64_t val)
{
    if(target == nullptr)
    {
        std::cerr << "HwDirectAccess::putScom target is nullptr" << std::endl;
        return -1;
    }

    std::cout << "HwDirectAccess::putScom executed" << std::endl;
    return 0; // TBD: Call to hwtransport::putScom
}
} // namespace hwaccess
