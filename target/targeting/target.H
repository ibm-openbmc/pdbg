#pragma once

#include <attributeenums.H>
#include <attributetraits.H>
#include <entitypath.H>
extern "C"
{
#include <libfdt.h>
}

#include <cstring>
#include <map>
#include <memory>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace TARGETING
{

// Owning pointers
using TargetUniquePtr = std::unique_ptr<Target>;
using TargetUniquePtrList = std::vector<TargetUniquePtr>;

// Non-owning pointers
using TargetPtr = Target*;
using ConstTargetPtr = const Target*;
using TargetPtrList = std::vector<TargetPtr>;
using ConstTargetPtrList = std::vector<ConstTargetPtr>;

class TargetDevtreeMap;

/**
 * @brief Variant type to hold optional attribute values for a Target.
 *
 * This type can hold various attribute value types, including integers,
 * strings, fixed-size char arrays, entity paths, enums, etc.
 */
using AttributeValue =
    std::variant<uint8_t, uint32_t, uint16_t, std::string, std::array<char, 64>,
                 EntityPath, HW_ACCESS_METHOD, TYPE, CLASS
                 // Add more types as you specialize more traits
                 >;

/**
 * @class Target
 * @brief Represents a device tree target with associated attributes.
 *
 * A Target corresponds to a specific hardware or logical component described
 * in the device tree. It provides typed accessors for getting and setting
 * attributes defined by AttributeTraits, with optional compile-time
 * enforcement of readable/writeable constraints.
 */
class Target
{
    friend class TargetDevtreeMap;
    friend class MockTarget;

  protected:
    /**
     * @brief Construct a Target instance from a device tree node.
     * @param fdt Pointer to the flattened device tree structure.
     * @param offset Node offset within the device tree.
     */
    Target(void* fdt, int offset);

  public:
    /// Destructor (defaulted).
    ~Target() = default;

    /**
     * @brief Get an attribute value.
     * @tparam A Attribute ID (must be readable).
     * @return The attribute value of the type defined by AttributeTraits<A>.
     */
    template <const ATTRIBUTE_ID A>
    typename AttributeTraits<A>::Type getAttr() const;

    /**
     * @brief Get an attribute value as a fixed-size array type.
     * @tparam A Attribute ID (must be readable).
     * @return The attribute value as TypeStdArr.
     */
    template <const ATTRIBUTE_ID A>
    typename AttributeTraits<A>::TypeStdArr getAttrAsArray() const;

    /**
     * @brief Try to get an attribute value.
     * @tparam A Attribute ID (must be readable).
     * @param o_attrValue Output parameter to hold the attribute value.
     * @return true if the attribute was retrieved, false otherwise.
     */
    template <const ATTRIBUTE_ID A>
    bool tryGetAttr(typename AttributeTraits<A>::Type& o_attrValue) const;

    /**
     * @brief Try to get an attribute value as an array.
     * @tparam A Attribute ID (must be readable).
     * @param o_attrValue Output parameter to hold the attribute value array.
     * @return true if the attribute was retrieved, false otherwise.
     */
    template <const ATTRIBUTE_ID A>
    bool tryGetAttr(typename AttributeTraits<A>::TypeStdArr& o_attrValue) const;

    /**
     * @brief Set an attribute value.
     * @tparam A Attribute ID (must be writeable).
     * @param attrValue The value to set.
     */
    template <const ATTRIBUTE_ID A>
    void setAttr(const typename AttributeTraits<A>::Type& attrValue);

    /**
     * @brief Set an attribute value as an array.
     * @tparam A Attribute ID (must be writeable).
     * @param attrValue The array value to set.
     */
    template <const ATTRIBUTE_ID A>
    void setAttr(const typename AttributeTraits<A>::TypeStdArr& attrValue);

    /**
     * @brief Try to set an attribute value.
     * @tparam A Attribute ID (must be writeable).
     * @param attrValue The value to set.
     * @return true if the attribute was set successfully, false otherwise.
     */
    template <const ATTRIBUTE_ID A>
    bool trySetAttr(const typename AttributeTraits<A>::Type& attrValue);

    /**
     * @brief Try to set an attribute value as an array.
     * @tparam A Attribute ID (must be writeable).
     * @param attrValue The array value to set.
     * @return true if the attribute was set successfully, false otherwise.
     */
    template <const ATTRIBUTE_ID A>
    bool trySetAttr(const typename AttributeTraits<A>::TypeStdArr& attrValue);

    /**
     * @brief Get the node offset in the device tree.
     * @return Node offset.
     */
    int getOffset() const noexcept;

  private:
    void* _fdt = nullptr;    ///< Pointer to device tree blob.
    int _offset = -1;        ///< Node offset in device tree.
    std::optional<std::map<ATTRIBUTE_ID, AttributeValue>>
        _volatileAttributes; ///< volatile in-memory attributes.

    /**
     * @brief Check if an volatile attribute exists.
     */
    bool isVolatileAttr(ATTRIBUTE_ID attrId) const
    {
        return _volatileAttributes && _volatileAttributes->contains(attrId);
    }

    /**
     * @brief Get an volatile attribute of a specific type.
     * @throw std::runtime_error if the attribute is not present or type
     * mismatched.
     */
    template <ATTRIBUTE_ID A>
    typename AttributeTraits<A>::Type getVolatileAttr() const;

    /**
     * @brief Set or update an optional attribute.
     */
    template <const ATTRIBUTE_ID A>
    void setVolatileAttr(const typename AttributeTraits<A>::Type& attrValue);

  protected:
    /**
     * @brief Retrieve a property from the device tree.
     *
     * Virtual to allow mocking in unit tests.
     *
     * @param fdt Pointer to device tree.
     * @param offset Node offset.
     * @param name Property name.
     * @return The property value as a span, or std::nullopt if not found.
     */
    virtual std::optional<std::span<const uint8_t>> fdtGetProperty(
        const void* fdt, int offset, const std::string& name) const;

  private:
    /**
     * @brief Set a property in the device tree from a span of bytes.
     */
    void fdtSetProperty(void* fdt, int offset, const std::string& name,
                        const std::span<const uint8_t>& data) const;

    /**
     * @brief Set a string property in the device tree.
     */
    void fdtSetStringProperty(void* fdt, int offset, const std::string& name,
                              const std::string& value) const;

    /**
     * @brief Set a raw property in the device tree.
     */
    void fdtSetRawProperty(void* fdt, int offset, const std::string& name,
                           const void* data, size_t size) const;

    /**
     * @brief Implementation for tryGetAttr.
     */
    template <ATTRIBUTE_ID A, typename T>
    bool tryGetAttrImpl(T& o_attrValue) const;

    /**
     * @brief Implementation for setAttr.
     */
    template <ATTRIBUTE_ID A, typename T> void setAttrImpl(const T& attrValue);

    /**
     * @brief Implementation for trySetAttr.
     */
    template <ATTRIBUTE_ID A, typename T>
    bool trySetAttrImpl(const T& attrValue);

    /**
     * @brief Try to get the name of an attribute.
     */
    template <ATTRIBUTE_ID A> std::optional<std::string> tryGetAttrName() const;
};

} // namespace TARGETING

#include "target_tmpl.H" // Template implementations
