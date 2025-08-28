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

transport::ByteVector buildGetScomCommand(uint64_t address)
{
    constexpr uint32_t wordCount = 0x00000004;
    constexpr uint32_t cmdWord = 0x0000A201;

    uint32_t words[4] = {
        htobe32(wordCount), htobe32(cmdWord),
        htobe32(static_cast<uint32_t>(address >> 32)),        // high 32 bits
        htobe32(static_cast<uint32_t>(address & 0xFFFFFFFF)), // low 32 bits
    };

    transport::ByteVector data(sizeof(words));
    std::memcpy(data.data(), words, sizeof(words));

    return data;
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
            /// direct acccess
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
            /// sbeifo access
            {
                std::cout << "sbefifo getscom command " << std::endl;
                transport::ByteVector cmd = buildGetScomCommand(0x50001);
                transport::ByteVector out;
                int timeout = 120;
                rc = transport::sendAndRecv(tgt, cmd, timeout, out);
                if (rc != 0)
                {
                    std::cerr << "Failed in sbefifo getscom " << std::endl;
                }
                else
                {
                    printBytes("cmd", cmd);
                    printBytes("value", out);
                }
            }
            {
                transport::ByteVector cmd = buildGetScomCommand(0x00020011);
                transport::ByteVector out;
                int timeout = 120;
                rc = transport::sendAndRecv(tgt, cmd, timeout, out);
                if (rc != 0)
                {
                    std::cerr << "Failed in sbefifo getscom " << std::endl;
                }
                else
                {
                    printBytes("cmd", cmd);
                    printBytes("value", out);
                }
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
