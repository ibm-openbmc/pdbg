#pragma once
#include <endian.h>

#include <cstdint>
#include <type_traits>

namespace util
{

// Integral types
template <typename T>
    requires std::is_integral_v<T>
constexpr T htobe_integral(T v)
{
    using Raw = T;
    using U = std::make_unsigned_t<Raw>;
    if constexpr (sizeof(Raw) == 2)
        return static_cast<T>(htobe16(static_cast<U>(v)));
    else if constexpr (sizeof(Raw) == 4)
        return static_cast<T>(htobe32(static_cast<U>(v)));
    else if constexpr (sizeof(Raw) == 8)
        return static_cast<T>(htobe64(static_cast<U>(v)));
    else
        static_assert(!sizeof(T), "Unsupported integral size");
}

template <typename T>
    requires std::is_integral_v<T>
constexpr T betoh_integral(T v)
{
    using Raw = T;
    using U = std::make_unsigned_t<Raw>;
    if constexpr (sizeof(Raw) == 2)
        return static_cast<T>(be16toh(static_cast<U>(v)));
    else if constexpr (sizeof(Raw) == 4)
        return static_cast<T>(be32toh(static_cast<U>(v)));
    else if constexpr (sizeof(Raw) == 8)
        return static_cast<T>(be64toh(static_cast<U>(v)));
    else
        static_assert(!sizeof(T), "Unsupported integral size");
}

// Enum types
template <typename T>
    requires std::is_enum_v<T>
constexpr T htobe_integral(T v)
{
    using Raw = std::underlying_type_t<T>;
    using U = std::make_unsigned_t<Raw>;
    Raw raw = static_cast<Raw>(v);

    if constexpr (sizeof(Raw) == 2)
        raw = static_cast<Raw>(htobe16(static_cast<U>(raw)));
    else if constexpr (sizeof(Raw) == 4)
        raw = static_cast<Raw>(htobe32(static_cast<U>(raw)));
    else if constexpr (sizeof(Raw) == 8)
        raw = static_cast<Raw>(htobe64(static_cast<U>(raw)));
    else
        static_assert(!sizeof(T), "Unsupported enum underlying size");

    return static_cast<T>(raw);
}

template <typename T>
    requires std::is_enum_v<T>
constexpr T betoh_integral(T v)
{
    using Raw = std::underlying_type_t<T>;
    using U = std::make_unsigned_t<Raw>;
    Raw raw = static_cast<Raw>(v);

    if constexpr (sizeof(Raw) == 2)
        raw = static_cast<Raw>(be16toh(static_cast<U>(raw)));
    else if constexpr (sizeof(Raw) == 4)
        raw = static_cast<Raw>(be32toh(static_cast<U>(raw)));
    else if constexpr (sizeof(Raw) == 8)
        raw = static_cast<Raw>(be64toh(static_cast<U>(raw)));
    else
        static_assert(!sizeof(T), "Unsupported enum underlying size");

    return static_cast<T>(raw);
}
} // namespace util
