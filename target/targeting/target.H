#pragma once

#include <attributeenums.H>
#include <attributetraits.H>
#include <entitypath.H>
#include <libfdt.h>

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

// optional attributes
using AttributeValue = std::variant<
    uint8_t, uint32_t, uint16_t, std::string, std::array<char, 64>, EntityPath
    // Add more types as you specialize more traits
    >;

class Target
{
    friend class TargetDevtreeMap;
    friend class MockTarget;

  protected:
    Target(void* fdt, int offset);

  public:
    ~Target() = default;

    template <const ATTRIBUTE_ID A>
    typename AttributeTraits<A>::Type getAttr() const;

    template <const ATTRIBUTE_ID A>
    typename AttributeTraits<A>::TypeStdArr getAttrAsArray() const;

    template <const ATTRIBUTE_ID A>
    bool tryGetAttr(typename AttributeTraits<A>::Type& o_attrValue) const;

    template <const ATTRIBUTE_ID A>
    bool tryGetAttr(typename AttributeTraits<A>::TypeStdArr& o_attrValue) const;

    template <const ATTRIBUTE_ID A>
    void setAttr(const typename AttributeTraits<A>::Type& attrValue);

    template <const ATTRIBUTE_ID A>
    void setAttr(const typename AttributeTraits<A>::TypeStdArr& attrValue);

    template <const ATTRIBUTE_ID A>
    bool trySetAttr(const typename AttributeTraits<A>::Type& attrValue);

    template <const ATTRIBUTE_ID A>
    bool trySetAttr(const typename AttributeTraits<A>::TypeStdArr& attrValue);

    int getOffset() const noexcept;

  private:
    void* _fdt = nullptr;
    int _offset = -1;
    std::optional<std::map<ATTRIBUTE_ID, AttributeValue>> _optAttributes;

    template <ATTRIBUTE_ID A>
    void addOptionalAttr(const typename AttributeTraits<A>::Type& value)
    {
        if (!_optAttributes)
        {
            _optAttributes.emplace();
        }
        (*_optAttributes)[A] = value;
    }

    bool hasOptionalAttr(ATTRIBUTE_ID attrId) const
    {
        return _optAttributes && _optAttributes->contains(attrId);
    }

    template <typename T> T getOptionalAttr(ATTRIBUTE_ID attrId) const
    {
        if (!hasOptionalAttr(attrId))
        {
            throw std::runtime_error("Optional attribute not present");
        }

        const auto& val = _optAttributes->at(attrId);
        if (const T* ptr = std::get_if<T>(&val))
        {
            return *ptr;
        }

        throw std::runtime_error("Optional attribute has unexpected type");
    }

    template <typename T>
    void setOptionalAttr(ATTRIBUTE_ID attrId, const T& attrValue)
    {
        if (!_optAttributes)
        {
            _optAttributes.emplace();
        }
        (*_optAttributes)[attrId] = attrValue;
    }

  protected:
    // adding virutal for unit test case purpose
    virtual std::optional<std::span<const uint8_t>> fdtGetProperty(
        const void* fdt, int offset, const std::string& name) const;

  private:
    void fdtSetProperty(void* fdt, int offset, const std::string& name,
                        const std::span<const uint8_t>& data) const;

    void fdtSetStringProperty(void* fdt, int offset, const std::string& name,
                              const std::string& value) const;

    void fdtSetRawProperty(void* fdt, int offset, const std::string& name,
                           const void* data, size_t size) const;
    template <ATTRIBUTE_ID A, typename T>
    bool tryGetAttrImpl(T& o_attrValue) const;

    template <ATTRIBUTE_ID A, typename T> void setAttrImpl(const T& attrValue);

    template <ATTRIBUTE_ID A, typename T>
    bool trySetAttrImpl(const T& attrValue);
};
} // namespace TARGETING

#include "target_tmpl.H" // Template implementations
