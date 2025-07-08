#pragma once

#include <predicatebase.H>
#include <target.H>

#include <memory>
#include <vector>

namespace TARGETING
{

class PredicatePostfixExpr : public PredicateBase
{
  public:
    PredicatePostfixExpr() = default;
    ~PredicatePostfixExpr() override = default;

    PredicatePostfixExpr& push(std::shared_ptr<PredicateBase> predicate);
    PredicatePostfixExpr& And();
    PredicatePostfixExpr& Or();
    PredicatePostfixExpr& Not();

    bool operator()(const TargetPtr& target) const override;

    // Disable copy
    PredicatePostfixExpr(const PredicatePostfixExpr&) = delete;
    PredicatePostfixExpr& operator=(const PredicatePostfixExpr&) = delete;

  private:
    enum class LogicalOp
    {
        Eval,
        And,
        Or,
        Not
    };

    struct Operation
    {
        LogicalOp logicalOp;                 ///< Logical operator to
                                             ///< apply to result stack
        std::shared_ptr<PredicateBase> pred; ///< Predicate to evaluate,
                                             ///< if logicalOp == EVAL
    };
    std::vector<Operation> _ops;

    bool evalStackItem(uintptr_t& item, const TargetPtr& target) const;
};

} // namespace TARGETING
