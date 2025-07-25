#pragma once

#include <attributeenums.H>
#include <attributetraits.H>
#include <entitypath.H>

#include <memory>
#include <optional>
#include <span>
#include <string>
#include <type_traits>

extern "C"
{
#include <libfdt.h>
}
#include <cstring>
#include <iostream>
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
    Target(const void* fdt, int offset) : _fdt(fdt), _offset(offset) {}

  public:
    ~Target() = default;

    template <const ATTRIBUTE_ID A>
    bool tryGetAttr(typename AttributeTraits<A>::Type& o_attrValue) const;

    std::string getName() const
    {
        const char* name = fdt_get_name(_fdt, _offset, nullptr);
        return name ? std::string(name) : "";
    }

    const void* _fdt = nullptr;
    int _offset = -1;
};

namespace internal
{

template <typename T>
bool tryGetAttrHelper(const void* fdt, int offset, const std::string& propName,
                      T& outVal)
{
    static_assert(std::is_trivially_copyable_v<T>,
                  "T must be trivially copyable or explicitly specialized");

    int len = 0;
    const void* prop = fdt_getprop(fdt, offset, propName.c_str(), &len);
    if (!prop || len < static_cast<int>(sizeof(T)))
    {
        return false;
    }

    std::memcpy(&outVal, prop, sizeof(T));
    return true;
}

// ------------------ Specializations (Declared here, implemented in .C)
template <>
bool tryGetAttrHelper<std::string>(const void* fdt, int offset,
                                   const std::string& propName,
                                   std::string& outVal);

template <>
bool tryGetAttrHelper<EntityPath>(const void* fdt, int offset,
                                  const std::string& propName,
                                  EntityPath& outVal);

} // namespace internal

template <const ATTRIBUTE_ID A>
bool Target::tryGetAttr(typename AttributeTraits<A>::Type& o_attrValue) const
{
    auto nameOpt = tryGetAttrName<A>();
    if (!nameOpt)
    {
        std::cout << "Attribute name not found for ID: " << A << std::endl;
        return false;
    }

    return internal::tryGetAttrHelper(_fdt, _offset, *nameOpt, o_attrValue);
}

} // namespace TARGETING
