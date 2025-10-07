#pragma once
#include "predicatebase.H"

#include <targeting/target.H>
#include <targeting/xmltohb/attributeenums.H>

namespace TARGETING
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

} // namespace TARGETING 
