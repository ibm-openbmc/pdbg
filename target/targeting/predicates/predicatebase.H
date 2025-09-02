#pragma once
#include <targeting/target.H>

namespace TARGETING
{

class Target;

/**
 * @class PredicateBase
 * @brief Abstract interface for defining target selection predicates.
 *
 * This class serves as the base interface for all predicate types used
 * to filter or match TARGETING::Target instances based on custom criteria.
 *
 * A predicate is a callable object that returns a boolean indicating
 * whether the given target satisfies the predicate condition.
 *
 * Derived classes must override the `operator()` method to implement
 * the desired evaluation logic.
 *
 * @note Copy operations are disabled to prevent accidental copying
 *       of predicate instances.
 */
class PredicateBase
{
  public:
    /**
     * @brief Virtual destructor.
     *
     * Ensures derived predicate classes can be deleted through
     * a base-class pointer without causing resource leaks.
     */
    virtual ~PredicateBase() = default;

    /**
     * @brief Evaluate the predicate for the given target.
     *
     * @param[in] i_pTarget  Constant pointer reference to the target
     *                       being evaluated.
     * @return `true` if the target matches the predicate, `false` otherwise.
     */
    virtual bool operator()(ConstTargetPtr& i_pTarget) const = 0;

  protected:
    /**
     * @brief Protected default constructor.
     *
     * Allows construction by derived classes while preventing direct
     * instantiation of this abstract class.
     */
    PredicateBase() = default;

    // Disable copy
    PredicateBase(const PredicateBase&) = delete; ///< Copy constructor deleted.
    PredicateBase&
        operator=(const PredicateBase&) = delete; ///< Copy assignment deleted.
};

} // namespace TARGETING
