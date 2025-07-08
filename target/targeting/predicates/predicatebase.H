#pragma once
#include <target.H>
namespace TARGETING
{

class Target;

// Interface class for predicates
class PredicateBase
{
  public:
    virtual ~PredicateBase() = default;

    virtual bool operator()(const TargetPtr& i_pTarget) const = 0;

  protected:
    PredicateBase() = default;
    // Disble copy
    PredicateBase(const PredicateBase&) = delete;
    PredicateBase& operator=(const PredicateBase&) = delete;
};

} // namespace TARGETING
