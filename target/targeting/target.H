#pragma once

#include <attributeenums.H>
#include <attributetraits.H>
#include <entitypath.H>
#include <libfdt.h>

#include <cstring>
#include <memory>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <type_traits>
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

class Target
{
    friend class TargetDevtreeMap;

  private:
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
    void setAttr(const typename AttributeTraits<A>::Type& i_attrValue);

    template <const ATTRIBUTE_ID A>
    void setAttr(const typename AttributeTraits<A>::TypeStdArr& i_attrValue);

    template <const ATTRIBUTE_ID A>
    bool trySetAttr(const typename AttributeTraits<A>::Type& i_attrValue);

    template <const ATTRIBUTE_ID A>
    bool trySetAttr(const typename AttributeTraits<A>::TypeStdArr& i_attrValue);

    int getOffset() const noexcept;

  private:
    void* _fdt = nullptr;
    int _offset = -1;

    std::optional<std::span<const uint8_t>> fdtGetProperty(
        const void* fdt, int offset, const std::string& name) const;

    void fdtSetProperty(void* fdt, int offset, const std::string& name,
                        const std::span<const uint8_t>& data) const;

    void fdtSetStringProperty(void* fdt, int offset, const std::string& name,
                              const std::string& value) const;

    void fdtSetRawProperty(void* fdt, int offset, const std::string& name,
                           const void* data, size_t size) const;
    template <ATTRIBUTE_ID A, typename T>
    bool tryGetAttrImpl(T& o_attrValue) const;

    template <ATTRIBUTE_ID A, typename T>
    void setAttrImpl(const T& i_attrValue);

    template <ATTRIBUTE_ID A, typename T>
    bool trySetAttrImpl(const T& i_attrValue);
};
} // namespace TARGETING

#include "target_tmpl.H" // Template implementations
