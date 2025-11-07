#include "hw_sbe_access.H"
#include <sbei/sbe_cmd_impl.H>

namespace hwaccess
{
HwSbeAccess& HwSbeAccess::getInstance()
{
    static HwSbeAccess sbeAccessInstance;
    return sbeAccessInstance;
}

int HwSbeAccess::getScom(ConstTargetPtr_t target, uint64_t addr, uint64_t& val)
{
    if(target == nullptr)
    {
        std::cerr << "HwSbeAccess::getScom target is nullptr" << std::endl;
        return -1;
    }

    std::cout << "HwSbeAccess::getScom executed" << std::endl;
    
    return sbei::getScom(target, addr, val);
}

int HwSbeAccess::putScom(ConstTargetPtr_t target, uint64_t addr, uint64_t val)
{
    if(target == nullptr)
    {
        std::cerr << "HwSbeAccess::putScom target is nullptr" << std::endl;
        return -1;
    }

    std::cout << "HwSbeAccess::putScom executed" << std::endl;

    return sbei::putScom(target, addr, val);
}
} //namespace hwaccess
