#include <entitypath.H>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <sstream>

namespace TARGETING
{

EntityPath::EntityPath(PathType type)
{
    setType(type);
    setSize(0);
}

EntityPath& EntityPath::removeLast()
{
    assert(getSize() > 0);
    _elements[getSize() - 1] = {};
    setSize(getSize() - 1);
    return *this;
}

EntityPath EntityPath::copyRemoveLast() const
{
    EntityPath copy = *this;
    copy.removeLast();
    return copy;
}

EntityPath& EntityPath::addLast(TYPE type, uint8_t instance)
{
    assert(getSize() < MAX_PATH_ELEMENTS);
    PathElement pe(type, instance);
    _elements[getSize()] = pe;
    setSize(getSize() + 1);
    return *this;
}

EntityPath EntityPath::copyAddLast(TYPE type, uint8_t instance) const
{
    EntityPath copy = *this;
    copy.addLast(type, instance);
    return copy;
}

bool EntityPath::operator==(const EntityPath& rhs) const
{
    return type() == rhs.type() && getSize() == rhs.getSize() &&
           std::equal(std::begin(_elements), std::begin(_elements) + getSize(),
                      std::begin(rhs._elements));
}

bool EntityPath::operator<(const EntityPath& rhs) const
{
    if (type() != rhs.type())
        return type() < rhs.type();

    int cmp =
        std::memcmp(_elements, rhs._elements,
                    std::min(getSize(), rhs.getSize()) * sizeof(PathElement));

    if (cmp != 0)
        return cmp < 0;

    return getSize() < rhs.getSize();
}

bool EntityPath::equals(const EntityPath& rhs, std::size_t count) const
{
    assert(count <= MAX_PATH_ELEMENTS);
    return type() == rhs.type() && count <= getSize() &&
           count <= rhs.getSize() &&
           std::equal(std::begin(_elements), std::begin(_elements) + count,
                      std::begin(rhs._elements));
}

const EntityPath::PathElement& EntityPath::operator[](std::size_t index) const
{
    assert(index < getSize());
    return _elements[index];
}

void EntityPath::replace(std::size_t index, const PathElement& element)
{
    assert(index < getSize());
    _elements[index] = element;
}

EntityPath::PathElement EntityPath::pathElementOfType(TYPE type) const
{
    for (uint32_t i = 0; i < getSize(); ++i)
    {
        if (_elements[i].type() == type)
        {
            return _elements[i];
        }
    }
    return {};
}

std::string EntityPath::pathTypeAsString() const
{
    switch (type())
    {
        case PathType::Device:
            return "Device:";
        case PathType::Affinity:
            return "Logical:";
        case PathType::Physical:
            return "Physical:";
        case PathType::Power:
            return "Power:";
        default:
            return "Unknown entity path type";
    }
}

std::string EntityPath::pathElementTypeAsString(TYPE type) const
{
    switch (type)
    {
        case TYPE_NA:
            return "NA";
        case TYPE_SYS:
            return "Sys";
        case TYPE_NODE:
            return "Node";
        case TYPE_PROC:
        case TYPE_MI:
            return "MI";
        case TYPE_MC:
            return "MC";
        case TYPE_MCC:
            return "MCC";
        case TYPE_OMI:
            return "OMI";
        case TYPE_OCMB_CHIP:
            return "OCMB";
        default:
            return "Unknown path type";
    }
}

std::string EntityPath::toString() const
{
    std::ostringstream oss;
    oss << pathTypeAsString();
    for (uint32_t i = 0; i < getSize(); ++i)
    {
        oss << '/' << pathElementTypeAsString(_elements[i].type())
            << static_cast<int>(_elements[i].instance);
    }
    return oss.str();
}

// In EntityPath.C
EntityPath EntityPath::fromBinary(std::span<const uint8_t> data)
{
    EntityPath path;

    if (data.size() < 1)
        return path;

    uint8_t header = data[0];
    uint8_t pathType = (header >> 4) & LOWER_NIBBLE_MASK;
    uint8_t elementCount = header & LOWER_NIBBLE_MASK;

    std::size_t expectedSize = 1 + 2 * elementCount;
    if (data.size() < expectedSize)
        return path;

    path.setType(static_cast<PathType>(pathType));

    auto elements = data.subspan(1, 2 * elementCount);
    for (size_t i = 0; i < elementCount; ++i)
    {
        TYPE type = static_cast<TYPE>(elements[i * 2]);
        uint8_t instance = elements[i * 2 + 1];
        path.addLast(type, instance); // this increments size correctly
    }

    return path;
}
} // namespace TARGETING
