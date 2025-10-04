#pragma once

#include <libfdt.h>
#include <targeting/common/entitypath.H>
#include <targeting/endian_utils.H>
#include <targeting/xmltohb/attributeenums.H>
#include <targeting/xmltohb/attributetraits.H>
#include <targeting/xmltohb/targAttrIdToName.H>

#include <cstring>
#include <iomanip>
#include <span>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace TARGETING
{

template <ATTRIBUTE_ID A> consteval bool isStringAttr()
{
    return std::is_same_v<typename AttributeTraits<A>::Type, std::string>;
}

template <ATTRIBUTE_ID A> consteval bool isEntityPathAttr()
{
    return std::is_same_v<typename AttributeTraits<A>::Type, EntityPath>;
}

template <ATTRIBUTE_ID A> consteval bool isStdArrayAttr()
{
    using T = typename AttributeTraits<A>::Type;
    return std::is_class_v<T> && requires(T t) {
                                     typename T::value_type;
                                     { t.size() } -> std::same_as<std::size_t>;
                                     {
                                         t.data()
                                     } -> std::same_as<typename T::value_type*>;
                                 };
}

template <ATTRIBUTE_ID A> consteval bool isCharArrayAttr()
{
    using T = typename AttributeTraits<A>::Type;
    return std::is_array_v<T> && std::is_same_v<std::remove_extent_t<T>, char>;
}

// ----------------------------------------
// Template Wrappers
// ----------------------------------------

template <const ATTRIBUTE_ID A>
typename AttributeTraits<A>::Type Target::getAttr() const
{
    (void)AttributeTraits<A>::readable;
    if constexpr (requires { AttributeTraits<A>::isVolatile; })
    {
        return getVolatileAttr<A>();
    }

    typename AttributeTraits<A>::Type value{};
    if (!tryGetAttrImpl<A>(value))
    {
        throw std::runtime_error("getAttr failed");
    }
    return value;
}

template <const ATTRIBUTE_ID A>
typename AttributeTraits<A>::TypeStdArr Target::getAttrAsArray() const
{
    (void)AttributeTraits<A>::readable;
    if constexpr (requires { AttributeTraits<A>::isVolatile; })
    {
        return getVolatileAttr<A>();
    }

    typename AttributeTraits<A>::TypeStdArr value{};
    if (!tryGetAttrImpl<A>(value))
    {
        throw std::runtime_error("getAttrAsArray failed");
    }
    return value;
}

template <const ATTRIBUTE_ID A>
bool Target::tryGetAttr(typename AttributeTraits<A>::Type& o_attrValue) const
{
    (void)AttributeTraits<A>::readable;
    if constexpr (requires { AttributeTraits<A>::isVolatile; })
    {
        o_attrValue = getVolatileAttr<A>();
        return true;
    }
    return tryGetAttrImpl<A>(o_attrValue);
}

template <const ATTRIBUTE_ID A>
bool Target::tryGetAttr(
    typename AttributeTraits<A>::TypeStdArr& o_attrValue) const
{
    (void)AttributeTraits<A>::readable;
    if constexpr (requires { AttributeTraits<A>::isVolatile; })
    {
        o_attrValue = getVolatileAttr<A>();
        return true;
    }
    return tryGetAttrImpl<A>(o_attrValue);
}

template <const ATTRIBUTE_ID A>
void Target::setAttr(const typename AttributeTraits<A>::Type& attrValue)
{
    (void)AttributeTraits<A>::writeable;
    if constexpr (requires { AttributeTraits<A>::isVolatile; })
    {
        setVolatileAttr<A>(attrValue);
        return;
    }
    trySetAttrImpl<A>(attrValue);
}

template <const ATTRIBUTE_ID A>
void Target::setAttr(const typename AttributeTraits<A>::TypeStdArr& attrValue)
{
    (void)AttributeTraits<A>::writeable;
    if constexpr (requires { AttributeTraits<A>::isVolatile; })
    {
        setVolatileAttr<A>(attrValue);
        return;
    }
    trySetAttrImpl<A>(attrValue);
}

template <const ATTRIBUTE_ID A>
bool Target::trySetAttr(const typename AttributeTraits<A>::Type& attrValue)
{
    (void)AttributeTraits<A>::writeable;
    if constexpr (requires { AttributeTraits<A>::isVolatile; })
    {
        setVolatileAttr<A>(attrValue);
        return true;
    }
    return trySetAttrImpl<A>(attrValue);
}

template <const ATTRIBUTE_ID A>
bool Target::trySetAttr(
    const typename AttributeTraits<A>::TypeStdArr& attrValue)
{
    (void)AttributeTraits<A>::writeable;
    if constexpr (requires { AttributeTraits<A>::isVolatile; })
    {
        setVolatileAttr<A>(attrValue);
        return true;
    }
    return trySetAttrImpl<A>(attrValue);
}

template <ATTRIBUTE_ID A>
typename AttributeTraits<A>::Type Target::getVolatileAttr() const
{
    // compile-time validation
    (void)AttributeTraits<A>::readable;
    (void)AttributeTraits<A>::isVolatile;

    using AttrType = typename AttributeTraits<A>::Type;

    if (!_volatileAttributes)
    {
        throw std::runtime_error("Volatile attribute is not set");
    }

    auto& attrMap = *_volatileAttributes;

    if (!isVolatileAttr(A))
    {
        throw std::runtime_error("Volatile attribute is not set");
    }

    const auto& val = attrMap.at(A);
    if (const AttrType* ptr = std::get_if<AttrType>(&val))
    {
        return *ptr;
    }

    throw std::runtime_error("Volatile attribute type mismatch");
}

template <const ATTRIBUTE_ID A>
void Target::setVolatileAttr(const typename AttributeTraits<A>::Type& attrValue)
{
    (void)AttributeTraits<A>::writeable;
    (void)AttributeTraits<A>::isVolatile;
    if (!_volatileAttributes)
    {
        _volatileAttributes.emplace();
    }
    (*_volatileAttributes)[A] = attrValue;
}

template <ATTRIBUTE_ID A, typename T>
bool Target::tryGetAttrImpl(T& o_attrValue) const
{
    if (auto nameOpt = tryGetAttrName<A>())
    {
        const std::string& propName = *nameOpt;
        auto propData = fdtGetProperty(_fdt, _offset, propName);
        if (!propData.has_value())
            return false;

        auto span = propData.value();
        using TraitType = typename AttributeTraits<A>::Type;
        using DecayedType =
            std::remove_cv_t<std::remove_reference_t<TraitType>>;

        // Handle string attributes
        if constexpr (isStringAttr<A>())
        {
            o_attrValue.assign(reinterpret_cast<const char*>(span.data()),
                               span.size());
            return true;
        }

        // Handle entity path attributes
        else if constexpr (isEntityPathAttr<A>())
        {
            o_attrValue = EntityPath::fromBinary(span);
            return o_attrValue.getSize() > 0;
        }

        // Handle fixed-size std::array attributes
        else if constexpr (isStdArrayAttr<A>())
        {
            if (span.size() > sizeof(TraitType))
                return false;
            std::memset(o_attrValue.data(), 0, sizeof(TraitType));
            std::memcpy(o_attrValue.data(), span.data(), span.size());
            return true;
        }

        // Handle fixed-size char array attributes
        else if constexpr (isCharArrayAttr<A>())
        {
            if (span.size() > sizeof(TraitType))
                return false;
            std::memset(&o_attrValue, 0, sizeof(TraitType));
            std::memcpy(&o_attrValue, span.data(), span.size());
            return true;
        }

        // Handle C-style arrays (e.g., uint8_t[4])
        else if constexpr (std::is_array_v<DecayedType>)
        {
            if (span.size() > sizeof(TraitType))
                return false;
            std::memset(&o_attrValue, 0, sizeof(TraitType));
            std::memcpy(&o_attrValue, span.data(), span.size());
            return true;
        }

        // Handle scalar / POD attributes
        else
        {
            if (span.size() > sizeof(TraitType))
                return false;
            TraitType temp{};
            std::memcpy(&temp, span.data(), span.size());

            if constexpr (std::is_integral_v<TraitType> &&
                          sizeof(TraitType) > 1)
            {
                temp = util::betoh_integral<TraitType>(temp);
            }
            else
            {
                static_assert(std::is_trivially_copyable_v<TraitType>,

                              "Unsupported attribute type for tryGetAttrImpl: "
                              "must be integral, enum, or trivially copyable.");
            }
            o_attrValue = static_cast<T>(temp);
            return true;
        }
    }
    return false;
}

template <ATTRIBUTE_ID A, typename T>
bool Target::trySetAttrImpl(const T& attrValue)
{
    if (auto nameOpt = tryGetAttrName<A>())
    {
        const std::string& propName = *nameOpt;

        if constexpr (isStringAttr<A>())
        {
            fdtSetStringProperty(_fdt, _offset, propName, attrValue);
        }
        else if constexpr (isEntityPathAttr<A>())
        {
            fdtSetProperty(_fdt, _offset, propName, attrValue.toBinary());
        }
        else if constexpr (isStdArrayAttr<A>())
        {
            const char* cstr = attrValue.data();
            size_t len = strnlen(cstr, attrValue.size());
            fdtSetRawProperty(_fdt, _offset, propName, cstr, len + 1);
        }
        else if constexpr (isCharArrayAttr<A>())
        {
            const char* cstr = reinterpret_cast<const char*>(&attrValue);
            size_t len = strnlen(cstr, sizeof(T));
            fdtSetRawProperty(_fdt, _offset, propName, cstr, len + 1);
        }
        else if constexpr (std::is_array_v<typename AttributeTraits<A>::Type>)
        {
            using ArrT = typename AttributeTraits<A>::Type;
            fdtSetRawProperty(_fdt, _offset, propName, attrValue, sizeof(ArrT));
        }
        else
        {
            // scalar/POD: convert to BE if integral multi-byte
            using TraitType = typename AttributeTraits<A>::Type;
            TraitType tmp = attrValue;
            if constexpr (std::is_integral_v<TraitType> &&
                          sizeof(TraitType) > 1)
            {
                tmp = util::htobe_integral<TraitType>(tmp);
            }
            else
            {
                static_assert(std::is_trivially_copyable_v<TraitType>,
                              "Unsupported attribute type for trySetAttrImpl: "
                              "must be integral, enum, or trivially copyable.");
            }
            fdtSetRawProperty(_fdt, _offset, propName, &tmp, sizeof(TraitType));
        }
    }
    return false;
}

template <ATTRIBUTE_ID A>
std::optional<std::string> Target::tryGetAttrName() const
{
    constexpr uint32_t i_attrId = static_cast<uint32_t>(A);

    if (auto it = g_rwAttrIdToNameMap.find(i_attrId);
        it != g_rwAttrIdToNameMap.end())
    {
        return std::string(it->second);
    }
    if (auto it = g_nonRwAttrIdToNameMap.find(i_attrId);
        it != g_nonRwAttrIdToNameMap.end())
    {
        return std::string(it->second);
    }

    return std::nullopt;
}
} // namespace TARGETING
