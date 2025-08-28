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
        typename AttributeTraits<A>::Type actual;
        return target->tryGetAttr<A>(actual) && (actual == _value);
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
