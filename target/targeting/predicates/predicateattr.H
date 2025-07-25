#pragma once

#include <attributeenums.H>
#include <attributetraits.H>
#include <predicatebase.H>
#include <target.H>

namespace TARGETING
{

class Target;

template <const ATTRIBUTE_ID A> class PredicateAttr : public PredicateBase
{
  public:
    explicit PredicateAttr() {}

    ~PredicateAttr() override = default;

    bool operator()(ConstTargetPtr& target) const override
    {
        typename AttributeTraits<A>::Type tmp; // intentionally uninitialized
        bool match = target->tryGetAttr<A>(tmp);
        return match;
    }

    // Disable copy
    PredicateAttr(const PredicateAttr&) = delete;
    PredicateAttr& operator=(const PredicateAttr&) = delete;
};
} // namespace TARGETING
