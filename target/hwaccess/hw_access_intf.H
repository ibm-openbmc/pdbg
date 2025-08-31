#pragma once

#include <target.H>
#include <attributeenums.H>
#include <attributetraits.H>

namespace hwaccess
{

using HwAccessMethod   = TARGETING::AttributeTraits<TARGETING::ATTR_HW_ACCESS_METHOD>::Type;
using ConstTargetPtr_t = TARGETING::ConstTargetPtr;

/**
 * @class HwAccessIntf
 * @brief Provides static interface methods to perform register operations via hwaccess.
 *
 * Exceptions will be thrown when register operations are attempted without
 * setting HwAccessMethod. The setHwAccessMethod is provided by the target service.
 * User is expected to catch and handle the exceptions.
 */
class HwAccessIntf
{
public:
    /**
     * @brief Fetch a CFAM register.
     *
     * @param[in]  target   Target on which the operation needs to be done.
     * @param[in]  addr     Address of the CFAM register to fetch.
     * @param[out] val      Retrieved data will be stored here.
     *
     * @return Return code of the operation from the transport layer.
     */
    static int getCfamRegister(ConstTargetPtr_t target, uint32_t addr, uint32_t& val);

    /**
     * @brief Write to a CFAM register.
     *
     * @param[in] target   Target on which the operation needs to be done.
     * @param[in] addr     Address of the CFAM register to write.
     * @param[in] val      Data to be written into the register.
     *
     * @return Return code of the operation from the transport layer.
     */
    static int putCfamRegister(ConstTargetPtr_t target, uint32_t addr, uint32_t val);

    /**
     * @brief Fetch a SCOM register.
     *
     * @param[in]  target   Target on which the operation needs to be done.
     * @param[in]  addr     Address of the SCOM register to fetch.
     * @param[out] val      Retrieved data will be stored here.
     *
     * @return Return code of the operation from the transport layer.
     */
    static int getScomRegister(ConstTargetPtr_t target, uint64_t addr, uint64_t& val);

    /**
     * @brief Write to a SCOM register.
     *
     * @param[in] target   Target on which the operation needs to be done.
     * @param[in] addr     Address of the SCOM register to write.
     * @param[in] val      Data to be written into the register.
     *
     * @return Return code of the operation from the transport layer.
     */
    static int putScomRegister(ConstTargetPtr_t target, uint64_t addr, uint64_t val);

    /**
     * @brief Fetch the HwAccessPtr relevant to the use case.
     *
     * @param[in] accessMethod   Access method (e.g., direct or sbefifo).
     *
     * @return Pointer to the requested hardware access object.
     */
    static void* getHwAccessPtr(HwAccessMethod& accessMethod);
};

} // namespace hwaccess
