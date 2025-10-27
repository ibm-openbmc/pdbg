#pragma once

#include "predicatebase.H"

#include <targeting/target.H>
#include <targeting/xmltohb/attributeenums.H>
#include <targeting/xmltohb/attributetraits.H>

namespace TARGETING
{

/**
 * @brief Predicate to check if a specific attribute has a given value.
 *
 * This template class derives from PredicateBase and evaluates whether a
 * target's attribute (identified by ATTRIBUTE_ID) matches a specified value.
 *
 * @tparam A The attribute ID (of type ATTRIBUTE_ID) to be checked.
 */
template <const ATTRIBUTE_ID A> class PredicateAttrVal : public PredicateBase
{
  public:
    /**
     * @brief Constructor that sets the expected attribute value.
     *
     * @param value The value to compare against the target's attribute.
     */
    explicit PredicateAttrVal(const typename AttributeTraits<A>::Type& value) :
        _value(value)
    {}

    template <size_t N>
    explicit PredicateAttrVal(const std::array<char, N>& arr)
    {
        static_assert(N <= sizeof(_value),
                      "Array too large for attribute value");
        std::memset(_value, 0, sizeof(_value)); // ensure null-termination
        std::memcpy(_value, arr.data(), N);
    }

    /**
     * @brief Virtual destructor.
     */
    ~PredicateAttrVal() override = default;

    /**
     * @brief Checks if the target's attribute matches the expected value.
     *
     * @param target Pointer to the constant target to be tested.
     * @return true if the attribute exists and equals the expected value.
     * @return false if the attribute does not exist or does not match.
     */
    bool operator()(ConstTargetPtr& target) const override
    {
        using AttrType = typename AttributeTraits<A>::Type;
        AttrType actual{};

        if (!target->tryGetAttr<A>(actual))
            return false;

        if constexpr (std::is_array_v<AttrType>)
        {
            // Compare full array contents (safe for char[N])
            return std::memcmp(actual, _value, sizeof(_value)) == 0;
        }
        else if constexpr (requires(AttrType a, AttrType b) { a == b; })
        {
            // Use natural equality when available
            return actual == _value;
        }
        else
        {
            // Optional: log or assert unsupported types
            static_assert(
                std::is_array_v<AttrType> ||
                    requires(AttrType a, AttrType b) { a == b; },
                "Unsupported attribute type for PredicateAttrVal comparison");
            return false;
        }
    }

    /// @brief Copy constructor is deleted (class is non-copyable).
    PredicateAttrVal(const PredicateAttrVal&) = delete;

    /// @brief Copy assignment operator is deleted (class is non-copyable).
    PredicateAttrVal& operator=(const PredicateAttrVal&) = delete;

  private:
    /// @brief The expected attribute value to match against.
    typename AttributeTraits<A>::Type _value;
};

} // namespace TARGETING
