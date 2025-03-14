//
// Created by M on 18.11.2024.
//

#include "WhileStatementNode.h"
#include "StatementSequenceNode.h"
#include "util/datastructures/ast/base_blocks/ExpressionNode.h"
#include "util/datastructures/ast/NodeVisitor.h"

void WhileStatementNode::accept(NodeVisitor &visitor)
{
    visitor.visit(*this);
}

string WhileStatementNode::to_string() const
{

    return "WHILE " + condition_->to_string() + " DO\n" + statements_->to_string() + "\n\tEND";
}

WhileStatementNode::WhileStatementNode(FilePos pos, std::unique_ptr<ExpressionNode> condition, std::unique_ptr<StatementSequenceNode> statements) : StatementNode(NodeType::while_statement, pos), condition_(std::move(condition)), statements_(std::move(statements)) {}

ExpressionNode *WhileStatementNode::get_expr() {
    return condition_.get();
}

StatementSequenceNode *WhileStatementNode::get_statements() {
    return statements_.get();
}