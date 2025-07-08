#pragma once

#include "attributeenums.H"
#include "entitypath.H"
#include "target.H"
extern "C"
{
#include <libfdt.h>
}
#include <association.H>
#include <generator.H>
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
    explicit TargetDevtreeMap(const void* fdt);

    TargetPtr getParentOf(const TargetPtr& child,
                          const AssociationType type) const;

    TargetPtr toTarget(const EntityPath& i_entityPath) const;

    TARGETING::generator<TargetPtr> getAssociated(
        const Target* source, const AssociationType type = childByAffinity,
        const RecursionLevel recursionLevel = immediate,
        const PredicateBase* predicate = nullptr) const;
    TargetPtr getTopLevelTarget() const noexcept;

  private:
    void indexAllNodes();

    EntityPath parseEntityPathProperty(int offset, const char* propName) const;

  private:
    const void* _fdt;
    int _rootOffset = -1;
    std::map<EntityPath, int> _phyPathToNode;
    std::map<EntityPath, int> _affinityPathToNode;
};
} // namespace TARGETING
