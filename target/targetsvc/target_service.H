#pragma once

#include "target_devtree_map.H"
#include "dtree_loader.H"
#include <targeting/association.H>
#include <targeting/predicates/predicatebase.H>
#include <targeting/target.H>

#include <cstdint>

namespace TARGETING
{
using HwAccessMethod = AttributeTraits<ATTR_HW_ACCESS_METHOD>::Type;
using HwAccessPtrType = AttributeTraits<ATTR_HW_ACCESS_PTR>::Type;

/**
 * @class TargetService
 * @brief Provides a singleton interface to access and query target objects from a device tree.
 *
 * TargetService is responsible for loading the device tree, mapping it into
 * target objects, and providing query APIs such as associations, parent lookup,
 * and top-level target retrieval.
 *
 * This class uses TargetDevtreeMap internally to organize and search targets
 * efficiently.
 */
class TargetService
{
  public:
    /**
     * @brief Get the singleton instance of TargetService.
     *
     * @return Reference to the singleton TargetService object.
     */
    static TargetService& instance();

    /**
     * @brief Initialize the service by loading a DTB (Device Tree Blob) file.
     *
     * @param[in] dtbPath Path to the DTB file to load.
     *
     * This method loads the device tree into memory and prepares
     * internal structures for target queries.
     */
    void init(const std::string& dtbPath);

    /**
     * @brief Get the underlying FDT (Flattened Device Tree) pointer.
     *
     * @return Pointer to the loaded FDT, or nullptr if not loaded.
     */
    void* getFDT() const noexcept
    {
        return _loader ? _loader->fdt() : nullptr;
    }

    /**
     * @brief Get the targets associated with a given source target.
     *
     * @param[in] source         Source target to find associations from.
     * @param[in] type           Association type (default: childByPhysical).
     * @param[in] recursionLevel Recursion level for search (default: immediate).
     * @param[in] predicate      Optional predicate to filter associated targets.
     *
     * @return List of associated targets matching the given criteria.
     */
    TargetPtrList getAssociated(ConstTargetPtr source,
                                const AssociationType type = childByPhysical,
                                const RecursionLevel recursionLevel = immediate,
                                const PredicateBase* predicate = nullptr) const;

    /**
     * @brief Get the parent target of a given child target.
     *
     * @param[in] child Child target pointer.
     * @param[in] type  Association type for parent lookup (default:* parentByPhysical).
     *
     * @return Parent target pointer, or nullptr if not found.
     */
    TargetPtr getParentOf(ConstTargetPtr child,
                          const AssociationType type = parentByPhysical) const;

    /**
     * @brief Convert an EntityPath to a Target object.
     *
     * @param[in] i_entityPath EntityPath to resolve.
     *
     * @return Target pointer corresponding to the given EntityPath, or nullptr if not found.
     */
    TargetPtr toTarget(const EntityPath& i_entityPath) const;

    /**
     * @brief Get the top-level (root) target in the device tree.
     *
     * @return Pointer to the top-level target, or nullptr if not found.
     */
    TargetPtr getTopLevelTarget() const;

    /**
     * @brief Set the hardware access method for a specified target
     *
     * @param[in] target HwTarget to set the access method
     * @param[in] accessMethod HwAccessMethod to use
     *
     */
    void setHwAccessMethod(TargetPtr target, HwAccessMethod accessMethod);

  private:
    /**
     * @brief Private constructor for singleton pattern.
     */
    TargetService() = default;

    /**
     * @brief Private destructor for singleton pattern.
     */
    ~TargetService() = default;

    // Disable copy and assignment
    TargetService(const TargetService&) = delete;
    TargetService& operator=(const TargetService&) = delete;

    int _rootOffset = -1;  ///< Offset of the root node in the FDT.
    std::unique_ptr<DeviceTreeLoader> _loader; ///< Loader for reading the DTB file.
    std::unique_ptr<TargetDevtreeMap> _targetMap; ///< Map for fast target lookups.
};
} // namespace TARGETING

