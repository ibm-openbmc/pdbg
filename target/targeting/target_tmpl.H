#pragma once

#include <attributeenums.H>
#include <attributetraits.H>
#include <entitypath.H>
#include <libfdt.h>

#include <cstring>
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
    return tryGetAttrImpl<A>(o_attrValue);
}

template <const ATTRIBUTE_ID A>
bool Target::tryGetAttr(
    typename AttributeTraits<A>::TypeStdArr& o_attrValue) const
{
    return tryGetAttrImpl<A>(o_attrValue);
}

template <const ATTRIBUTE_ID A>
void Target::setAttr(const typename AttributeTraits<A>::Type& i_attrValue)
{
    setAttrImpl<A>(i_attrValue);
}

template <const ATTRIBUTE_ID A>
void Target::setAttr(const typename AttributeTraits<A>::TypeStdArr& i_attrValue)
{
    setAttrImpl<A>(i_attrValue);
}

template <const ATTRIBUTE_ID A>
bool Target::trySetAttr(const typename AttributeTraits<A>::Type& i_attrValue)
{
    return trySetAttrImpl<A>(i_attrValue);
}

template <const ATTRIBUTE_ID A>
bool Target::trySetAttr(
    const typename AttributeTraits<A>::TypeStdArr& i_attrValue)
{
    return trySetAttrImpl<A>(i_attrValue);
}

// ----------------------------------------
// Internal Template Logic
// ----------------------------------------

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

        if constexpr (isStringAttr<A>())
        {
            o_attrValue.assign(reinterpret_cast<const char*>(span.data()),
                               span.size());
            return true;
        }
        else if constexpr (isEntityPathAttr<A>())
        {
            o_attrValue = EntityPath::fromBinary(span);
            return o_attrValue.getSize() > 0;
        }
        else if constexpr (isStdArrayAttr<A>())
        {
            if (span.size() > sizeof(T))
                return false;
            std::memset(o_attrValue.data(), 0, sizeof(T));
            std::memcpy(o_attrValue.data(), span.data(), span.size());
            return true;
        }
        else if constexpr (isCharArrayAttr<A>())
        {
            if (span.size() > sizeof(T))
                return false;
            std::memset(&o_attrValue, 0, sizeof(T));
            std::memcpy(&o_attrValue, span.data(), span.size());
            return true;
        }
        else
        {
            if (span.size() != sizeof(T))
                return false;
            std::memcpy(&o_attrValue, span.data(), sizeof(T));
            return true;
        }
    }
    return false;
}

template <ATTRIBUTE_ID A, typename T>
void Target::setAttrImpl(const T& i_attrValue)
{
    if (auto nameOpt = tryGetAttrName<A>())
    {
        const std::string& propName = *nameOpt;

        if constexpr (isStringAttr<A>())
        {
            fdtSetStringProperty(_fdt, _offset, propName, i_attrValue);
        }
        else if constexpr (isEntityPathAttr<A>())
        {
            fdtSetProperty(_fdt, _offset, propName, i_attrValue.toBinary());
        }
        else if constexpr (isStdArrayAttr<A>())
        {
            const char* cstr = i_attrValue.data();
            size_t len = strnlen(cstr, i_attrValue.size());
            fdtSetRawProperty(_fdt, _offset, propName, cstr, len + 1);
        }
        else if constexpr (isCharArrayAttr<A>())
        {
            const char* cstr = reinterpret_cast<const char*>(&i_attrValue);
            size_t len = strnlen(cstr, sizeof(T));
            fdtSetRawProperty(_fdt, _offset, propName, cstr, len + 1);
        }
        else
        {
            fdtSetRawProperty(_fdt, _offset, propName, &i_attrValue, sizeof(T));
        }
    }
}

template <ATTRIBUTE_ID A, typename T>
bool Target::trySetAttrImpl(const T& i_attrValue)
{
    try
    {
        setAttrImpl<A>(i_attrValue);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

} // namespace TARGETING
