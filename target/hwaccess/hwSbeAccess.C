#include "hwSbeAccess.H"

namespace hwaccess
{
HwSbeAccess& HwSbeAccess::getInstance()
{
    static HwSbeAccess sbeAccessInstance;
    return sbeAccessInstance;
}

int HwSbeAccess::getScom(TargetPtr_t target, uint64_t addr, uint64_t& val)
{
    if(target == nullptr)
    {
        return -1;
    }

    std::cout << "HwSbeAccess::getScom executed" << std::endl;
    
    return 0; // TBD: Call to SBEI::getScom
}

int HwSbeAccess::putScom(TargetPtr_t target, uint64_t addr, uint64_t val)
{
    if(target == nullptr)
    {
        return -1;
    }

    std::cout << "HwSbeAccess::putScom executed" << std::endl;

    return 0; // TBD: Call to SBEI::putScom
}
} //namespace hwaccess
