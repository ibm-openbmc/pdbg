#pragma once

#include "predicatebase.H"

#include <targeting/target.H>

#include <memory>
#include <vector>

namespace TARGETING
{

/**
 * @class PredicatePostfixExpr
 * @brief Represents a logical predicate expression in postfix notation.
 *
 * This class allows building complex logical expressions composed of
 * multiple predicates combined with logical AND, OR, and NOT operations.
 * The predicates are stored in a vector as postfix operations and evaluated
 * in sequence.
 *
 * Example:
 * @code
 * PredicatePostfixExpr expr;
 * expr.push(predA).push(predB).And();
 * bool result = expr(target);
 * @endcode
 */
class PredicatePostfixExpr : public PredicateBase
{
  public:
    /**
     * @brief Default constructor.
     */
    PredicatePostfixExpr() = default;

    /**
     * @brief Default destructor.
     */
    ~PredicatePostfixExpr() override = default;

    /**
     * @brief Push a predicate onto the expression stack.
     *
     * @param predicate Shared pointer to a PredicateBase instance.
     * @return Reference to this object for chaining.
     */
    PredicatePostfixExpr& push(std::shared_ptr<PredicateBase> predicate);

    /**
     * @brief Push a logical AND operation.
     *
     * @return Reference to this object for chaining.
     */
    PredicatePostfixExpr& And();

    /**
     * @brief Push a logical OR operation.
     *
     * @return Reference to this object for chaining.
     */
    PredicatePostfixExpr& Or();

    /**
     * @brief Push a logical NOT operation.
     *
     * @return Reference to this object for chaining.
     */
    PredicatePostfixExpr& Not();

    /**
     * @brief Evaluate the postfix expression against a target.
     *
     * @param target Constant pointer to the target to test.
     * @return True if the expression evaluates to true for the target,
     *         false otherwise.
     */
    bool operator()(ConstTargetPtr& target) const override;

    // Disable copy
    PredicatePostfixExpr(const PredicatePostfixExpr&) = delete;
    PredicatePostfixExpr& operator=(const PredicatePostfixExpr&) = delete;

  private:
    /**
     * @brief Enumeration of supported logical operations.
     */
    enum class LogicalOp
    {
        Eval, ///< Evaluate a predicate
        And,  ///< Logical AND
        Or,   ///< Logical OR
        Not   ///< Logical NOT
    };

    /**
     * @struct Operation
     * @brief Represents a single postfix operation.
     */
    struct Operation
    {
        LogicalOp logicalOp; ///< Logical operator to apply
        std::shared_ptr<PredicateBase>
            pred;            ///< Predicate to evaluate if logicalOp == Eval
    };

    /** @brief List of postfix operations making up the expression. */
    std::vector<Operation> _ops;

    /**
     * @brief Evaluate a single stack item during expression evaluation.
     *
     * @param item  Reference to the item (treated as boolean) to evaluate.
     * @param target Target to evaluate the predicate against.
     * @return True on successful evaluation, false otherwise.
     */
    bool evalStackItem(uintptr_t& item, ConstTargetPtr& target) const;
};

} // namespace TARGETING
