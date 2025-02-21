//
// Created by M on 21.11.2024.
//

#include "ExpressionNode.h"
#include "parser/ast/base_blocks/IdentNode.h"
#include "parser/ast/base_blocks/SelectorNode.h"

#include "parser/ast/NodeVisitor.h"

ExpressionNode::ExpressionNode(FilePos pos, const NodeType type) : Node(type, pos) {}

int ExpressionNode::get_precedence() const
{
    return precedence_;
}

SourceOperator ExpressionNode::token_to_op(TokenType t)
{
    switch (t)
    {
    case TokenType::op_plus:
        return SourceOperator::PLUS;
    case TokenType::op_minus:
        return SourceOperator::MINUS;
    case TokenType::op_or:
        return SourceOperator::OR;
    case TokenType::op_times:
        return SourceOperator::MULT;
    case TokenType::op_div:
        return SourceOperator::DIV;
    case TokenType::op_mod:
        return SourceOperator::MOD;
    case TokenType::op_and:
        return SourceOperator::AND;
    case TokenType::op_not:
        return SourceOperator::NOT;
    case TokenType::op_eq:
        return SourceOperator::EQ;
    case TokenType::op_lt:
        return SourceOperator::LT;
    case TokenType::op_leq:
        return SourceOperator::LEQ;
    case TokenType::op_gt:
        return SourceOperator::GT;
    case TokenType::op_geq:
        return SourceOperator::GEQ;
    case TokenType::op_neq:
        return SourceOperator::NEQ;
    default:
        return SourceOperator::NO_OPERATOR;
    }
}

void ExpressionNode::print_operator(ostream &stream, SourceOperator op)
{
    switch (op)
    {
    case PLUS:
        stream << "+";
        break;
    case MINUS:
    case NEG:
        stream << "-";
        break;
    case OR:
        stream << "OR";
        break;
    case MULT:
        stream << "*";
        break;
    case DIV:
        stream << "DIV";
        break;
    case MOD:
        stream << "MOD";
        break;
    case AND:
        stream << "&";
        break;
    case NOT:
        stream << "~";
        break;
    case EQ:
        stream << "=";
        break;
    case NEQ:
        stream << "#";
        break;
    case LT:
        stream << "<";
        break;
    case LEQ:
        stream << "<=";
        break;
    case GT:
        stream << ">";
        break;
    case GEQ:
        stream << ">=";
        break;
    case PAREN:
        return;
    case NO_OPERATOR:
    default:
        stream << "<ERROR_OP>";
        break;
    }
}

std::ostream &operator<<(ostream &stream, const SourceOperator op)
{
    ExpressionNode::print_operator(stream, op);
    return stream;
}

int ExpressionNode::op_to_precedence(SourceOperator op)
{
    switch (op)
    {
    // Lowest Precedence --> Top-level boolean operators like =,#,<,...
    case EQ:
    case NEQ:
    case LT:
    case LEQ:
    case GT:
    case GEQ:
        return 0;
    // Precedence of 1 --> Term Operators like +,- (the operator),OR
    case PLUS:
    case MINUS:
    case OR:
        return 1;
    // Precedence of 2 --> Factor Operators like *,DIV,MOD,...
    case MULT:
    case DIV:
    case MOD:
    case AND:
        return 2;
    // Highest Precedence --> Unary Operators like - (the sign), ~ and also parantheses
    case NEG:
    case NOT:
    case PAREN:
        return 3;
    // Error Precedence
    case NO_OPERATOR:
    default:
        return -1;
    }
}

void ExpressionNode::set_value(long value)
{
    value_ = std::optional<long>(value);
}

std::optional<long> ExpressionNode::get_value()
{
    return value_;
}

void ExpressionNode::set_types(TypeInfo formal, TypeInfo actual, TypeNode *node)
{
    formal_type = std::move(formal);
    actual_type = std::move(actual);
    type_node = node;
}

TypeInfo ExpressionNode::get_formal_type()
{
    return formal_type;
}

TypeNode *ExpressionNode::get_type_node()
{
    return type_node;
}

TypeInfo ExpressionNode::get_actual_type()
{
    return actual_type;
}

UnaryExpressionNode::UnaryExpressionNode(FilePos pos, std::unique_ptr<ExpressionNode> expr, SourceOperator op) : ExpressionNode(pos, NodeType::unary_expression), op_(op), expr_(std::move(expr))
{
    precedence_ = op_to_precedence(op); // Should normally be equal to 2
}

void UnaryExpressionNode::accept(NodeVisitor &visitor)
{
    visitor.visit(*this);
}

void UnaryExpressionNode::print(ostream &stream) const
{

    if (op_ == SourceOperator::PAREN)
    {
        stream << "(" << *expr_ << ")";
    }
    else
    {
        stream << op_ << " " << *(expr_);
    }
}

ExpressionNode *UnaryExpressionNode::get_expr()
{
    return expr_.get();
}

SourceOperator UnaryExpressionNode::get_op()
{
    return op_;
}

BinaryExpressionNode::BinaryExpressionNode(FilePos pos, std::unique_ptr<ExpressionNode> lhs, SourceOperator op, std::unique_ptr<ExpressionNode> rhs) : ExpressionNode(pos, NodeType::binary_expression), op_(op), lhs_(std::move(lhs)), rhs_(std::move(rhs))
{
    precedence_ = op_to_precedence(op);
}

void BinaryExpressionNode::accept(NodeVisitor &visitor)
{
    visitor.visit(*this);
}

void BinaryExpressionNode::print(ostream &stream) const
{
    stream << *lhs_ << " " << op_ << " " << *rhs_;
}

BinaryExpressionNode *BinaryExpressionNode::insert_rightmost(SourceOperator op, std::unique_ptr<ExpressionNode> new_rhs)
{
    rhs_ = std::make_unique<BinaryExpressionNode>(rhs_->pos(), std::move(rhs_), op, std::move(new_rhs));
    return dynamic_cast<BinaryExpressionNode *>(rhs_.get());
}

ExpressionNode *BinaryExpressionNode::get_rhs()
{
    return rhs_.get();
}

ExpressionNode *BinaryExpressionNode::get_lhs()
{
    return lhs_.get();
}

SourceOperator BinaryExpressionNode::get_op()
{
    return op_;
}

IdentSelectorExpressionNode::IdentSelectorExpressionNode(FilePos pos, std::unique_ptr<IdentNode> ident, std::unique_ptr<SelectorNode> selector) : ExpressionNode(pos, NodeType::ident_selector_expression), ident_(std::move(ident)), selector_(std::move(selector)) {}

void IdentSelectorExpressionNode::accept(NodeVisitor &visitor)
{
    visitor.visit(*this);
}

void IdentSelectorExpressionNode::print(ostream &stream) const
{
    stream << *ident_;

    if (selector_)
    {
        stream << *selector_;
    }
}

IdentNode *IdentSelectorExpressionNode::get_identifier()
{
    return ident_.get();
}

SelectorNode *IdentSelectorExpressionNode::get_selector()
{
    return selector_.get();
}