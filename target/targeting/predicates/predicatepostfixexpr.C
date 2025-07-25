#include <predicatepostfixexpr.H>

#include <cassert>

namespace TARGETING
{

PredicatePostfixExpr&
    PredicatePostfixExpr::push(std::shared_ptr<PredicateBase> predicate)
{
    Operation op = {LogicalOp::Eval, std::move(predicate)};
    _ops.emplace_back(std::move(op));
    return *this;
}

PredicatePostfixExpr& PredicatePostfixExpr::And()
{
    Operation op = {LogicalOp::And, nullptr};
    _ops.emplace_back(std::move(op));
    return *this;
}

PredicatePostfixExpr& PredicatePostfixExpr::Or()
{
    Operation op = {LogicalOp::Or, nullptr};
    _ops.emplace_back(op);
    return *this;
}

PredicatePostfixExpr& PredicatePostfixExpr::Not()
{
    Operation op = {LogicalOp::Not, nullptr};
    _ops.emplace_back(op);
    return *this;
}

bool PredicatePostfixExpr::evalStackItem(uintptr_t& item,
                                         ConstTargetPtr& target) const
{
    if (item > 1)
    {
        auto* pred = reinterpret_cast<PredicateBase*>(item);
        assert(pred != nullptr); // optional
        bool result = (*pred)(target);
        item = result ? 1 : 0;
        return result;
    }
    else
    {
        return item;
    }
}

bool PredicatePostfixExpr::operator()(ConstTargetPtr& target) const
{
    assert(target != nullptr);
    std::vector<uintptr_t> stack;

    for (const auto& op : _ops)
    {
        switch (op.logicalOp)
        {
            case LogicalOp::Eval:
            {
                assert(op.pred); // Optional safety check
                stack.push_back(reinterpret_cast<uintptr_t>(op.pred.get()));
                break;
            }
            case LogicalOp::And:
            {
                assert(stack.size() >= 2);
                auto rhs = stack.back();
                stack.pop_back();
                auto& lhs = stack.back();
                if (!evalStackItem(lhs, target))
                {
                    lhs = 0;
                }
                else
                {
                    bool r = evalStackItem(rhs, target);
                    lhs = (lhs && r);
                }
                break;
            }
            case LogicalOp::Or:
            {
                assert(stack.size() >= 2);
                auto rhs = stack.back();
                stack.pop_back();
                auto& lhs = stack.back();
                if (evalStackItem(lhs, target))
                {
                    lhs = 1;
                }
                else
                {
                    bool r = evalStackItem(rhs, target);
                    lhs = (lhs || r);
                }
                break;
            }
            case LogicalOp::Not:
            {
                assert(stack.size() >= 1);
                auto& top = stack.back();
                bool r = evalStackItem(top, target);
                top = !r;
                break;
            }
            default:
                assert(false && "Unknown logical operator");
        }
    }

    if (stack.empty())
        return true;

    assert(stack.size() == 1);
    return evalStackItem(stack.back(), target);
}

} // namespace TARGETING
