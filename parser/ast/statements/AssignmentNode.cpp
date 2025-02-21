//
// Created by M on 21.11.2024.
//

#include "AssignmentNode.h"
#include "parser/ast/base_blocks/IdentNode.h"
#include "parser/ast/base_blocks/SelectorNode.h"
#include "parser/ast/base_blocks/ExpressionNode.h"
#include "parser/ast/NodeVisitor.h"


void AssignmentNode::accept(NodeVisitor &visitor)
{
    visitor.visit(*this);
}

void AssignmentNode::print(ostream &stream) const
{
    stream << *variable_;

    if (selector_)
    {
        stream << *selector_;
    }

    stream << " := " << *expr_;
}

AssignmentNode::AssignmentNode(FilePos pos, std::unique_ptr<IdentNode> variable, std::unique_ptr<SelectorNode> selector, std::unique_ptr<ExpressionNode> expr)
    : StatementNode(NodeType::assignment, pos), variable_(std::move(variable)), selector_(std::move(selector)), expr_(std::move(expr)) {}

IdentNode *AssignmentNode::get_variable() {
    return variable_.get();
}

SelectorNode *AssignmentNode::get_selector() {
    return selector_.get();
}

ExpressionNode *AssignmentNode::get_expr() {
    return expr_.get();
};