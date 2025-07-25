#pragma once

#include <attributeenums.H>
#include <predicatebase.H>
#include <target.H>

namespace targeting
{

class PredicateIsFunctional : public PredicateBase
{
  public:
    PredicateIsFunctional() = default;
    ~PredicateIsFunctional() override = default;

    bool operator()(ConstTargetPtr& target) const override
    {
        return target && target->getAttr<ATTR_HWAS_STATE>().functional;
    }

    // Disable copy
    PredicateIsFunctional(const PredicateIsFunctional&) = delete;
    PredicateIsFunctional& operator=(const PredicateIsFunctional&) = delete;
};

} // namespace targeting
