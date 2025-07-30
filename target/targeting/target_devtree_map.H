#pragma once

#include "attributeenums.H"
#include "entitypath.H"
#include "target.H"
extern "C"
{
#include <libfdt.h>
}
#include <association.H>
#include <predicatebase.H>

#include <map>
namespace TARGETING
{
class TargetService;
class TargetDevtreeMap
{
    friend class TargetService;

  public:
    ~TargetDevtreeMap() = default;

  private:
    explicit TargetDevtreeMap(void* fdt);

    TargetPtr getParentOf(ConstTargetPtr child, const AssociationType type);

    TargetPtr toTarget(const EntityPath& entityPath);

    TargetPtrList getAssociated(ConstTargetPtr source,
                                const AssociationType type = childByAffinity,
                                const RecursionLevel recursionLevel = immediate,
                                const PredicateBase* predicate = nullptr);

    TargetPtr getTopLevelTarget();

  private:
    void indexAllNodes();

    EntityPath parseEntityPathProperty(int offset, const char* propName) const;

    TargetPtr getOrCreateTarget(const EntityPath& path, int offset);

  private:
    void* _fdt;
    int _rootOffset = -1;
    std::map<EntityPath, int> _phyPathToNode;
    std::map<EntityPath, int> _affinityPathToNode;
    std::map<EntityPath, TargetUniquePtr> _accessedTargets;
};
} // namespace TARGETING
