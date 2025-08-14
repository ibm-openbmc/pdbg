#pragma once

#include <attributeenums.H>
#include <attributetraits.H>
#include <predicatebase.H>
#include <target.H>

namespace TARGETING
{

class Target;

/**
 * @class PredicateAttr
 * @brief A predicate that evaluates whether a target contains a given attribute.
 *
 * This template predicate checks if a specific attribute (identified by
 * the ATTRIBUTE_ID template parameter) is present in a given target.  
 * It leverages the `tryGetAttr` method of the Target to test for the
 * attribute's existence without throwing exceptions.
 *
 * @tparam A The ATTRIBUTE_ID constant representing the attribute to check.
 */
template <const ATTRIBUTE_ID A>
class PredicateAttr : public PredicateBase
{
  public:
    /**
     * @brief Default constructor.
     *
     * Initializes a PredicateAttr instance for the given attribute ID.
     */
    explicit PredicateAttr() {}

    /**
     * @brief Destructor.
     */
    ~PredicateAttr() override = default;

    /**
     * @brief Evaluates the predicate for the given target.
     *
     * This operator checks whether the target contains the attribute
     * identified by the template parameter `A`. It does this by attempting
     * to retrieve the attribute value using `Target::tryGetAttr`.
     *
     * @param[in] target A constant smart pointer reference to the target object.
     * @return `true` if the attribute exists in the target, otherwise `false`.
     */
    bool operator()(ConstTargetPtr& target) const override
    {
        typename AttributeTraits<A>::Type tmp; // intentionally uninitialized
        bool match = target->tryGetAttr<A>(tmp);
        return match;
    }

    // Disable copy
    /**
     * @brief Deleted copy constructor.
     */
    PredicateAttr(const PredicateAttr&) = delete;

    /**
     * @brief Deleted copy assignment operator.
     */
    PredicateAttr& operator=(const PredicateAttr&) = delete;
};

} // namespace TARGETING

