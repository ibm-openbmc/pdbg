#pragma once

#include <targeting/xmltohb/attributeenums.H>
#include <targeting/target.H>
#include <targeting/common/entitypath.H>
extern "C"
{
#include <libfdt.h>
}
#include <targeting/association.H>
#include <targeting/predicates/predicatebase.H>

#include <map>

namespace TARGETING
{

class TargetService;

/**
 * @class TargetDevtreeMap
 * @brief Builds and manages a mapping from EntityPaths to target objects
 *        using a flattened device tree (FDT) representation.
 *
 * This class is responsible for parsing the provided FDT, indexing
 * all nodes by their physical and affinity paths, and creating
 * `Target` objects on demand. It is primarily accessed via
 * `TargetService`.
 */
class TargetDevtreeMap
{
    friend class TargetService;

  public:
    /**
     * @brief Destructor
     *
     * Cleans up resources used by the mapping. Target objects are
     * automatically freed via their smart pointers.
     */
    ~TargetDevtreeMap() = default;

  private:
    /**
     * @brief Constructor
     *
     * Parses the provided FDT and prepares internal mappings for
     * target lookups.
     *
     * @param[in] fdt  Pointer to the loaded flattened device tree.
     */
    explicit TargetDevtreeMap(void* fdt);

    /**
     * @brief Get the parent target of a given child target.
     *
     * @param[in] child  Child target whose parent is requested.
     * @param[in] type   Association type used to determine the parent.
     * @return Pointer to the parent target, or nullptr if none found.
     */
    TargetPtr getParentOf(ConstTargetPtr child, const AssociationType type);

    /**
     * @brief Convert an EntityPath to its corresponding target.
     *
     * @param[in] entityPath  The entity path to look up.
     * @return Pointer to the target object, or nullptr if not found.
     */
    TargetPtr toTarget(const EntityPath& entityPath);

    /**
     * @brief Get a list of targets associated with a source target.
     *
     * @param[in] source          Source target for the association.
     * @param[in] type            Association type to filter on.
     * @param[in] recursionLevel  Whether to include only immediate
     *                            associations or recurse through the
     *                            hierarchy.
     * @param[in] predicate       Optional filter for matching targets.
     * @return List of associated target pointers.
     */
    TargetPtrList getAssociated(ConstTargetPtr source,
                                const AssociationType type = childByAffinity,
                                const RecursionLevel recursionLevel = immediate,
                                const PredicateBase* predicate = nullptr);

    /**
     * @brief Get the top-level (root) target in the hierarchy.
     *
     * @return Pointer to the top-level target.
     */
    TargetPtr getTopLevelTarget();

  private:
    /**
     * @brief Build indexes for all nodes in the FDT.
     *
     * This method scans the device tree and populates the
     * `_phyPathToNode` and `_affinityPathToNode` maps.
     */
    void indexAllNodes();

    /**
     * @brief Parse an entity path from a property of an FDT node.
     *
     * @param[in] offset    Node offset in the FDT.
     * @param[in] propName  Property name containing the entity path.
     * @return Parsed entity path object.
     */
    EntityPath parseEntityPathProperty(int offset, const char* propName) const;

    /**
     * @brief Get an existing target or create a new one.
     *
     * If the target for the given entity path exists in
     * `_accessedTargets`, it is returned; otherwise, a new
     * `Target` object is created and stored.
     *
     * @param[in] path    Entity path for the target.
     * @param[in] offset  Corresponding node offset in the FDT.
     * @return Pointer to the target object.
     */
    TargetPtr getOrCreateTarget(const EntityPath& path, int offset);

  private:
    /** Pointer to the loaded flattened device tree. */
    void* _fdt;

    /** Offset of the root node in the FDT. */
    int _rootOffset = -1;

    /** Mapping from physical entity path to FDT node offset. */
    std::map<EntityPath, int> _phyPathToNode;

    /** Mapping from affinity entity path to FDT node offset. */
    std::map<EntityPath, int> _affinityPathToNode;

    /** Cache of created Target objects, keyed by entity path. */
    std::map<EntityPath, TargetUniquePtr> _accessedTargets;
};

} // namespace TARGETING

