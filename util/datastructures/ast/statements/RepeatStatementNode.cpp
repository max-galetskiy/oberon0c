//
// Created by M on 18.11.2024.
//

#include "RepeatStatementNode.h"
#include "util/datastructures/ast/base_blocks/ExpressionNode.h"
#include "StatementSequenceNode.h"
#include "util/datastructures/ast/NodeVisitor.h"

void RepeatStatementNode::accept(NodeVisitor &visitor)
{
    visitor.visit(*this);
}

string RepeatStatementNode::to_string() const
{
    return "REPEAT\n" + statements_->to_string() + "\nUNTIL " + condition_->to_string();
}

RepeatStatementNode::RepeatStatementNode(FilePos pos, std::unique_ptr<ExpressionNode> condition, std::unique_ptr<StatementSequenceNode> statements) : StatementNode(NodeType::repeat_statement, pos), condition_(std::move(condition)), statements_(std::move(statements)) {}

ExpressionNode *RepeatStatementNode::get_expr() {
    return condition_.get();
}

StatementSequenceNode *RepeatStatementNode::get_statements() {
    return statements_.get();
}
