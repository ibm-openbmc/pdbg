#pragma once

#include <attributeenums.H>
#include <attributetraits.H>
#include <predicatebase.H>
#include <target.H>

namespace TARGETING
{

template <const ATTRIBUTE_ID A> class PredicateAttrVal : public PredicateBase
{
  public:
    explicit PredicateAttrVal(const typename AttributeTraits<A>::Type& value) :
        _value(value)
    {}

    ~PredicateAttrVal() override = default;

    bool operator()(ConstTargetPtr& target) const override
    {
        typename AttributeTraits<A>::Type actual;
        return target->tryGetAttr<A>(actual) && (actual == _value);
    }

    // Disabl copy
    PredicateAttrVal(const PredicateAttrVal&) = delete;
    PredicateAttrVal& operator=(const PredicateAttrVal&) = delete;

  private:
    typename AttributeTraits<A>::Type _value;
};

} // namespace TARGETING
