#pragma once

#include <targeting/xmltohb/attributeenums.H>

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

/// @brief Mask for extracting the lower nibble of a byte.
static constexpr uint8_t LOWER_NIBBLE_MASK = 0x0F;

/// @brief Mask for extracting the upper nibble of a byte.
static constexpr uint8_t UPPER_NIBBLE_MASK = 0xF0;

/**
 * @class EntityPath
 * @brief Represents a hierarchical path to a hardware entity.
 *
 * EntityPath is used to uniquely identify hardware targets within the system
 * through a sequence of path elements. Each path element contains a type and
 * instance number. The class also stores a path type and size metadata.
 */
class EntityPath
{
  public:
    /// @brief Maximum allowed number of path elements in the entity path.
    static constexpr std::size_t MAX_PATH_ELEMENTS = 10;

    /**
     * @enum PathType
     * @brief Enumeration of possible entity path types.
     */
    enum class PathType : uint8_t
    {
        NA       = 0x00, ///< Not applicable
        Affinity = 0x01, ///< Affinity path
        Physical = 0x02, ///< Physical path
        Device   = 0x03, ///< Device path
        Power    = 0x04, ///< Power path
    };

    /**
     * @struct PathElement
     * @brief Represents a single element in the entity path.
     *
     * A path element consists of a type (encoded in 8 bits) and an instance ID.
     */
    struct PathElement
    {
        uint8_t type8{};   ///< Encoded type value
        uint8_t instance{};///< Instance number of this element

        /// @brief Default constructor.
        PathElement() = default;

        /**
         * @brief Construct a path element with type and instance.
         * @param t Type of the path element.
         * @param inst Instance number.
         */
        PathElement(TYPE t, uint8_t inst) :
            type8(static_cast<uint8_t>(t)), instance(inst)
        {}

        /**
         * @brief Get the type of the path element.
         * @return Type as a TYPE enum.
         */
        TYPE type() const
        {
            return static_cast<TYPE>(type8);
        }

        /**
         * @brief Set the type of the path element.
         * @param t The type to set.
         */
        void setType(TYPE t)
        {
            type8 = static_cast<uint8_t>(t);
        }

        /// @brief Three-way comparison operator.
        auto operator<=>(const PathElement&) const = default;
    } __attribute__((packed));

    /// @brief Default constructor.
    EntityPath() = default;

    /**
     * @brief Construct an EntityPath with a specified path type.
     * @param type The path type to assign.
     */
    explicit EntityPath(PathType type);

    /// @brief Create a copy of this EntityPath without the last element.
    EntityPath copyRemoveLast() const;

    /// @brief Create a copy of this EntityPath with an extra element added.
    EntityPath copyAddLast(TYPE type, uint8_t instance) const;

    /// @brief Remove the last element from the path.
    EntityPath& removeLast();

    /// @brief Add a new element to the path.
    EntityPath& addLast(TYPE type, uint8_t instance);

    /// @brief Equality operator.
    bool operator==(const EntityPath& rhs) const;

    /// @brief Less-than operator for ordering.
    bool operator<(const EntityPath& rhs) const;

    /**
     * @brief Compare two entity paths up to a given number of elements.
     * @param rhs The other EntityPath.
     * @param count Number of elements to compare.
     * @return true if they match for the given count, false otherwise.
     */
    bool equals(const EntityPath& rhs, std::size_t count) const;

    /**
     * @brief Get a path element at the given index.
     * @param index Index in the path.
     * @return Reference to the PathElement.
     */
    const PathElement& operator[](std::size_t index) const;

    /**
     * @brief Replace a path element at a given index.
     * @param index The index to replace.
     * @param element The new PathElement.
     */
    void replace(std::size_t index, const PathElement& element);

    /**
     * @brief Find the first path element of the given type.
     * @param type The type to search for.
     * @return The matching PathElement, or a default if not found.
     */
    PathElement pathElementOfType(TYPE type) const;

    /**
     * @brief Get the number of elements in the path.
     * @return The size as a uint16_t.
     */
    uint16_t getSize() const
    {
        uint16_t size = static_cast<uint16_t>(_header & 0x0F);
        return size;
    }

    /**
     * @brief Set the number of elements in the path.
     * @param size The new size (lower nibble).
     */
    void setSize(uint8_t size)
    {
        _header = (_header & UPPER_NIBBLE_MASK) | (size & LOWER_NIBBLE_MASK);
    }

    /**
     * @brief Get the path type from the upper nibble.
     * @return The path type.
     * @note Will assert if the value is invalid.
     */
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

    /**
     * @brief Set the path type (upper nibble).
     * @param type The path type to set.
     */
    void setType(PathType type)
    {
        _header = (_header & LOWER_NIBBLE_MASK) |
                  ((static_cast<uint8_t>(type) & LOWER_NIBBLE_MASK) << 4);
    }

    /**
     * @brief Get a string representation of the path type.
     * @return String representing the path type.
     */
    std::string pathTypeAsString() const;

    /**
     * @brief Get a string representation of a path element type.
     * @param type The element type.
     * @return String representation.
     */
    std::string pathElementTypeAsString(TYPE type) const;

    /**
     * @brief Convert the entity path to a human-readable string.
     * @return String representation.
     */
    std::string toString() const;

    /**
     * @brief Construct an EntityPath from binary data.
     * @param data Binary data span.
     * @return Constructed EntityPath.
     */
    static EntityPath fromBinary(std::span<const uint8_t> data);

  private:
    uint8_t _header = 0; ///< Upper nibble = PathType, lower nibble = Size.

    /// @brief Array of path elements making up the entity path.
    PathElement _elements[MAX_PATH_ELEMENTS];
} __attribute__((packed));

/// @brief Verify that PathElement is tightly packed (2 bytes).
static_assert(sizeof(EntityPath::PathElement) == 2,
              "PathElement should be 2 bytes");

/// @brief Verify that EntityPath is tightly packed (header + elements).
static_assert(sizeof(EntityPath) == 1 + EntityPath::MAX_PATH_ELEMENTS *
                                            sizeof(EntityPath::PathElement),
              "EntityPath should be tightly packed");

} // namespace TARGETING
