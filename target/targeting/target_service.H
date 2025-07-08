#pragma once

#include <association.H>
#include <dtree_loader.H>
#include <generator.H>
#include <predicatebase.H>
#include <target.H>
#include <target_devtree_map.H>

#include <cstdint>

namespace TARGETING
{
class TargetService
{
  public:
    // Singleton accessor
    static TargetService& instance();

    // Public API
    void init(const std::string& dtbPath);

    void* getFDT() const noexcept
    {
        return _loader ? _loader->fdt() : nullptr;
    }

    TARGETING::generator<TargetPtr> getAssociated(
        const Target* source, const AssociationType type = childByAffinity,
        const RecursionLevel recursionLevel = immediate,
        const PredicateBase* predicate = nullptr) const;

    TargetPtr getParentOf(const TargetPtr& child,
                          const AssociationType type = parentByAffinity) const;

    TargetPtr toTarget(const EntityPath& i_entityPath) const;

    TargetPtr getTopLevelTarget() const noexcept;

  private:
    TargetService() = default;
    ~TargetService() = default;

    TargetService(const TargetService&) = delete;
    TargetService& operator=(const TargetService&) = delete;

    bool isInitialized() const noexcept
    {
        return _initialized;
    }

    int _rootOffset = -1;
    std::unique_ptr<DeviceTreeLoader> _loader;
    std::unique_ptr<TargetDevtreeMap> _targetMap;

    bool _initialized{false};
};
} // namespace TARGETING
