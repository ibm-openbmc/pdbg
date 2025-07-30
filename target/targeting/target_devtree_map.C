#include <target_devtree_map.H>

namespace TARGETING
{

constexpr const char* ATTR_PHYS_PATH_PROP = "ATTR_PHYS_PATH";
constexpr const char* ATTR_AFFINITY_PATH_PROP = "ATTR_AFFINITY_PATH";

TargetDevtreeMap::TargetDevtreeMap(void* fdt) : _fdt(fdt)
{
    indexAllNodes();
}

TargetPtr TargetDevtreeMap::getParentOf(ConstTargetPtr child,
                                        const AssociationType type)
{
    if (!child)
    {
        std::cerr << "Error: input childptr is set to null  \n";
        return nullptr;
    }

    // Validate association type
    if (type != parentByPhysical && type != parentByAffinity)
    {
        std::cerr << "Invalid association type for getParentOf(): " << type
                  << std::endl;
        return nullptr;
    }

    // Select appropriate path map
    const auto& pathMap =
        (type == parentByPhysical) ? _phyPathToNode : _affinityPathToNode;

    EntityPath childPath;
    bool gotPath = false;

    // Dispatch based on association type
    if (type == parentByPhysical)
    {
        gotPath = child->tryGetAttr<ATTR_PHYS_PATH>(childPath);
    }
    else if (type == parentByAffinity)
    {
        gotPath = child->tryGetAttr<ATTR_AFFINITY_PATH>(childPath);
    }

    if (!gotPath)
    {
        std::cerr
            << "Error: Failed to find either ATTR_PHYS_PATH or  ATTR_AFFINITY_PATH property \n";
        return nullptr;
    }

    EntityPath parentPath = childPath.copyRemoveLast();

    auto it = pathMap.find(parentPath);
    if (it == pathMap.end())
    {
        return nullptr;
    }

    return getOrCreateTarget(parentPath, it->second);
}

TargetPtr TargetDevtreeMap::toTarget(const EntityPath& i_entityPath)
{
    auto it = _phyPathToNode.find(i_entityPath);
    if (it == _phyPathToNode.end())
    {
        std::cerr << "Invalid entity path\n";
        return nullptr;
    }

    return getOrCreateTarget(i_entityPath, it->second);
}

TargetPtrList TargetDevtreeMap::getAssociated(
    const ConstTargetPtr source, AssociationType type,
    RecursionLevel recursionLevel, const PredicateBase* predicate)
{
    using enum AssociationType;
    using enum RecursionLevel;

    TargetPtrList result;

    if (!source)
    {
        std::cerr << "source pointer is not set\n";
        return result;
    }

    const std::map<EntityPath, int>* pathMap = nullptr;
    EntityPath sourcePath;

    switch (type)
    {
        case childByPhysical:
        case parentByPhysical:
            if (!source->tryGetAttr<ATTR_PHYS_PATH>(sourcePath))
            {
                return result;
            }
            pathMap = &_phyPathToNode;
            break;
        case childByAffinity:
        case parentByAffinity:
            if (!source->tryGetAttr<ATTR_AFFINITY_PATH>(sourcePath))
            {
                return result;
            }
            pathMap = &_affinityPathToNode;
            break;
    }

    if (sourcePath.getSize() == 0 || !pathMap)
    {
        return result;
    }

    if (type == childByPhysical || type == childByAffinity)
    {
        for (const auto& [path, offset] : *pathMap)
        {
            bool match = false;
            if (recursionLevel == immediate)
            {
                match = path.getSize() == sourcePath.getSize() + 1 &&
                        path.equals(sourcePath, sourcePath.getSize());
            }
            else if (recursionLevel == all)
            {
                match = path.getSize() > sourcePath.getSize() &&
                        path.equals(sourcePath, sourcePath.getSize());
            }

            if (match)
            {
                auto tgt = getOrCreateTarget(path, offset);
                if (tgt && (!predicate ||
                            (*predicate)(const_cast<ConstTargetPtr&>(tgt))))
                {
                    result.push_back(tgt);
                }
            }
        }
    }
    else // parentBy*
    {
        EntityPath current = sourcePath.copyRemoveLast();
        while (current.getSize() > 0)
        {
            if (const auto it = pathMap->find(current); it != pathMap->end())
            {
                auto tgt = getOrCreateTarget(current, it->second);
                if (tgt && (!predicate ||
                            (*predicate)(const_cast<ConstTargetPtr&>(tgt))))
                {
                    result.push_back(tgt);
                }

                if (recursionLevel == immediate)
                {
                    break;
                }
            }
            current = current.copyRemoveLast();
        }
    }

    return result;
}

void TargetDevtreeMap::indexAllNodes()
{
    _rootOffset = fdt_path_offset(_fdt, "/");
    if (_rootOffset < 0)
    {
        std::cerr << "Error: failed to find root node \n";
        throw std::runtime_error("Failed to find root node");
    }
    int offset = -1;
    while ((offset = fdt_next_node(_fdt, offset, nullptr)) >= 0)
    {
        // Parse physical path
        EntityPath physPath =
            parseEntityPathProperty(offset, ATTR_PHYS_PATH_PROP);
        if (physPath.getSize() > 0 &&
            physPath.type() == EntityPath::PathType::Physical)
        {
            _phyPathToNode.emplace(physPath, offset);
        }

        // Parse affinity path
        EntityPath affinityPath =
            parseEntityPathProperty(offset, ATTR_AFFINITY_PATH_PROP);
        if (affinityPath.getSize() > 0 &&
            affinityPath.type() == EntityPath::PathType::Affinity)
        {
            _affinityPathToNode.emplace(affinityPath, offset);
        }
    }
}

EntityPath TargetDevtreeMap::parseEntityPathProperty(int offset,
                                                     const char* propName) const
{
    int len = 0;
    const uint8_t* prop = reinterpret_cast<const uint8_t*>(
        fdt_getprop(_fdt, offset, propName, &len));
    if (!prop || len <= 0)
    {
        throw std::runtime_error(
            std::string("Failed to find ") + propName + " for the target");
    }

    std::span<const uint8_t> data(prop, static_cast<size_t>(len));
    return EntityPath::fromBinary(data);
}

TargetPtr TargetDevtreeMap::getTopLevelTarget()
{
    if (_rootOffset < 0)
    {
        std::cerr << "Root offset not set\n";
        return nullptr;
    }

    EntityPath rootPath =
        parseEntityPathProperty(_rootOffset, ATTR_PHYS_PATH_PROP);
    return getOrCreateTarget(rootPath, _rootOffset);
}

TargetPtr
    TargetDevtreeMap::getOrCreateTarget(const EntityPath& path, int offset)
{
    auto it = _accessedTargets.find(path);
    if (it != _accessedTargets.end())
    {
        return it->second.get();
    }

    auto target = std::unique_ptr<Target>(new Target(_fdt, offset));
    TargetPtr rawPtr = target.get();
    _accessedTargets.emplace(path, std::move(target));
    return rawPtr;
}
} // namespace TARGETING
