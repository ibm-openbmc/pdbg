#include <targeting/predicates/predicateattrval.H>
#include <targeting/target.H>
#include <targeting/xmltohb/attributeenums.H>
#include <targeting/xmltohb/attributestructs.H>
#include <targeting/xmltohb/attributetraits.H>
#include <targetsvc/target_service.H>
#include <transport/transport.H>

#include <iomanip>
#include <iostream>

using namespace TARGETING;

void printBytes(std::string_view name, std::span<const std::byte> data)
{
    std::cout << name << " [size=" << data.size() << "]: ";
    for (auto b : data)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(b) << " ";
    }
    std::cout << std::dec << "\n";
}

int main()
{
    try
    {
        auto& ts = TargetService::instance();
        ts.init("/tmp/targeting_test.dtb");
        auto top = TargetService::instance().getTopLevelTarget();
        std::cout << "Test1: GetCFAM on proc0 target \n";
        PredicateAttrVal<ATTR_TYPE> pred(TYPE_PROC);
        for (auto&& tgt :
             ts.getAssociated(top, AssociationType::childByPhysical,
                              RecursionLevel::all, &pred))
        {
            uint32_t cfamvalue = 0;
            int rc = transport::getCfam(tgt, 0x2810, cfamvalue);
            if (rc != 0)
            {
                std::cerr << "failed in getCfam 0x2810 " << std::endl;
            }
            else
            {
                std::cout << "successfully read addr:0x2810 cfam value 0x"
                          << std::hex << cfamvalue << std::endl;
            }
            rc = transport::putCfam(tgt, 0x2810, cfamvalue);
            if (rc != 0)
            {
                std::cerr << "failed in putCfam 0x2810 " << std::endl;
            }
            else
            {
                std::cout << "successfully write addr:0x2810 cfam value 0x"
                          << std::hex << cfamvalue << std::endl;
            }
            rc = transport::getCfam(tgt, 0x2810, cfamvalue);
            if (rc != 0)
            {
                std::cerr << "failed in getCfam 0x2810 " << std::endl;
            }
            else
            {
                std::cout << "successfully read addr:0x2810 cfam value 0x"
                          << std::hex << cfamvalue << std::endl;
            }
            uint64_t scomvalue = 0;
            rc = transport::getScom(tgt, 0x50001, scomvalue);
            if (rc != 0)
            {
                std::cerr << "failed in getScom 0x50001 " << std::endl;
            }
            else
            {
                std::cout << "successfully read addr:0x50001 scom value 0x"
                          << std::hex << scomvalue << std::endl;
            }
            rc = transport::putScom(tgt, 0x50001, scomvalue);
            if (rc != 0)
            {
                std::cerr << "failed in getScom 0x50001 " << std::endl;
            }
            else
            {
                std::cout << "successfully write addr:0x50001 scom value 0x"
                          << std::hex << scomvalue << std::endl;
            }
            rc = transport::getScom(tgt, 0x50001, scomvalue);
            if (rc != 0)
            {
                std::cerr << "failed in getScom 0x50001 " << std::endl;
            }
            else
            {
                std::cout << "successfully read addr:0x50001 scom value 0x"
                          << std::hex << scomvalue << std::endl;
            }
            break;
        }
    }
    catch (std::exception& ex)
    {
        std::cerr << "Exception: " << ex.what() << "\n";
    }

    return 0;
}
