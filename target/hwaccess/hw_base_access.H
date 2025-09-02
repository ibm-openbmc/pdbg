#pragma once

#include <targeting/target.H>
#include <targeting/xmltohb/attributeenums.H>
#include <targeting/xmltohb/attributetraits.H>
#include <hwaccess/hw_access_intf.H>

namespace hwaccess
{
/**
 * @class HwBaseAccess
 * @brief Base class providing hardware access interface for CFAM and SCOM operations.
 *
 * This class defines the common interface for hardware access implementations.
 * Derived classes are expected to override SCOM operations.
 */
class HwBaseAccess 
{
protected:
    /**
     * @brief Protected default constructor to prevent direct instantiation.
     */
    HwBaseAccess() = default;

    /**
     * @brief Deleted copy constructor.
     */
    HwBaseAccess(const HwBaseAccess&) = delete;

    /**
     * @brief Deleted copy assignment operator.
     */
    HwBaseAccess& operator=(const HwBaseAccess&) = delete;

    /**
     * @brief Virtual destructor for safe polymorphic deletion.
     */
    virtual ~HwBaseAccess() = default;

public:
    /**
     * @brief Perform a CFAM read operation.
     *
     * @param[in]  target Target on which the operation needs to be done.
     * @param[in]  addr   Address to read from.
     * @param[out] val    Reference to store the read value.
     *
     * @return Status code (0 on success, non-zero on error).
     */
    int getCfam(ConstTargetPtr_t target, uint32_t addr, uint32_t& val);

    /**
     * @brief Perform a CFAM write operation.
     *
     * @param[in] target Target on which the operation needs to be done.
     * @param[in] addr   Address to write to.
     * @param[in] val    Value to write.
     *
     * @return Status code (0 on success, non-zero on error).
     */
    int putCfam(ConstTargetPtr_t target, uint32_t addr, uint32_t val);

    /**
     * @brief Perform a SCOM read operation (default implementation).
     *
     * @param[in]  target Target on which the operation needs to be done.
     * @param[in]  addr   SCOM address.
     * @param[out] val    Reference to store the read value.
     *
     * @return Status code (-1 by default).
     */
    virtual int getScom(ConstTargetPtr_t target, uint64_t addr, uint64_t& val) { return -1; }

    /**
     * @brief Perform a SCOM write operation (default implementation).
     *
     * @param[in] target Target on which the operation needs to be done.
     * @param[in] addr   SCOM address.
     * @param[in] val    Value to write.
     *
     * @return Status code (-1 by default).
     */
    virtual int putScom(ConstTargetPtr_t target, uint64_t addr, uint64_t val) { return -1; }
};
}  // namespace hwaccess
