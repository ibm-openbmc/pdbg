#pragma once

#include <attributeenums.H>

#include <array>
#include <cassert>
#include <compare>
#include <cstdint>
#include <iostream>
#include <span>
#include <string>
namespace TARGETING
{

class Target;
static constexpr uint8_t LOWER_NIBBLE_MASK = 0x0F;
static constexpr uint8_t UPPER_NIBBLE_MASK = 0xF0;

class EntityPath
{
  public:
    static constexpr std::size_t MAX_PATH_ELEMENTS = 10;

    enum class PathType : uint8_t
    {
        NA = 0x00,
        Affinity = 0x01,
        Physical = 0x02,
        Device = 0x03,
        Power = 0x04,
    };

    struct PathElement
    {
        uint8_t type8{};
        uint8_t instance{};
        PathElement() {};
        PathElement(TYPE t, uint8_t inst) :
            type8(static_cast<uint8_t>(t)), instance(inst)
        {}
        TYPE type() const
        {
            return static_cast<TYPE>(type8);
        }
        void setType(TYPE t)
        {
            type8 = static_cast<uint8_t>(t);
        }

        auto operator<=>(const PathElement&) const = default;
    } __attribute__((packed));

    EntityPath() = default;
    explicit EntityPath(PathType type);

    EntityPath copyRemoveLast() const;
    EntityPath copyAddLast(TYPE type, uint8_t instance) const;

    EntityPath& removeLast();
    EntityPath& addLast(TYPE type, uint8_t instance);

    bool operator==(const EntityPath& rhs) const;
    bool operator<(const EntityPath& rhs) const;

    bool equals(const EntityPath& rhs, std::size_t count) const;
    const PathElement& operator[](std::size_t index) const;
    void replace(std::size_t index, const PathElement& element);

    PathElement pathElementOfType(TYPE type) const;

    uint16_t getSize() const
    {
        uint16_t size = static_cast<uint16_t>(_header & 0x0F);
        return size;
    }

    // Set size (lower nibble)
    void setSize(uint8_t size)
    {
        _header = (_header & UPPER_NIBBLE_MASK) | (size & LOWER_NIBBLE_MASK);
    }

    // Get path type (upper nibble)
    PathType type() const
    {
        uint8_t raw = (_header >> 4) & LOWER_NIBBLE_MASK;

        switch (static_cast<PathType>(raw))
        {
            case PathType::Physical:
            case PathType::Affinity:
            case PathType::Device:
            case PathType::Power:
                return static_cast<PathType>(raw);
            default:
                std::cerr << "Invalid PathType value: " << static_cast<int>(raw)
                          << std::endl;
                assert(false && "Invalid PathType value");
        }
    }

    // Set path type (upper nibble)
    void setType(PathType type)
    {
        _header = (_header & LOWER_NIBBLE_MASK) |
                  ((static_cast<uint8_t>(type) & LOWER_NIBBLE_MASK) << 4);
    }

    std::string pathTypeAsString() const;
    std::string pathElementTypeAsString(TYPE type) const;
    std::string toString() const;
    static EntityPath fromBinary(std::span<const uint8_t> data);

  private:
    uint8_t _header = 0;

    PathElement _elements[MAX_PATH_ELEMENTS];
} __attribute__((packed));

// Size assertions
static_assert(sizeof(EntityPath::PathElement) == 2,
              "PathElement should be 2 bytes");
static_assert(sizeof(EntityPath) == 1 + EntityPath::MAX_PATH_ELEMENTS *
                                            sizeof(EntityPath::PathElement),
              "EntityPath should be tightly packed");

} // namespace TARGETING
