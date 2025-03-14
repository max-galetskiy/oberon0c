//
// Created by M on 21.11.2024.
//

#include "AssignmentNode.h"
#include "util/datastructures/ast/base_blocks/IdentNode.h"
#include "util/datastructures/ast/base_blocks/SelectorNode.h"
#include "util/datastructures/ast/base_blocks/ExpressionNode.h"
#include "util/datastructures/ast/NodeVisitor.h"


void AssignmentNode::accept(NodeVisitor &visitor)
{
    visitor.visit(*this);
}

string AssignmentNode::to_string() const {

    string s = variable_->to_string();

    if (selector_)
    {
        s += selector_->to_string();
    }

    s += " := " + expr_->to_string();
    return s;
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
}