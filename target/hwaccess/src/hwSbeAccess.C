#include "hwSbeAccess.H"

namespace HWACCESS
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
    // Call to SBEI::getScom
    
    return 0;
}

int HwSbeAccess::putScom(TargetPtr_t target, uint64_t addr, uint64_t val)
{
    if(target == nullptr)
    {
        return -1;
    }

    std::cout << "HwSbeAccess::putScom executed" << std::endl;
   // Call to SBEI::putScom

   return 0;
}
} //namespace HWACCESS
