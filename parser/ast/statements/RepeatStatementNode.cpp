//
// Created by M on 18.11.2024.
//

#include "RepeatStatementNode.h"
#include "parser/ast/base_blocks/ExpressionNode.h"
#include "StatementSequenceNode.h"
#include "parser/ast/NodeVisitor.h"

void RepeatStatementNode::accept(NodeVisitor &visitor)
{
    visitor.visit(*this);
}

void RepeatStatementNode::print(ostream &stream) const
{
    stream << "REPEAT\n" << *statements_ << "\nUNTIL " << *condition_;
}

RepeatStatementNode::RepeatStatementNode(FilePos pos, std::unique_ptr<ExpressionNode> condition, std::unique_ptr<StatementSequenceNode> statements) : StatementNode(NodeType::repeat_statement, pos), condition_(std::move(condition)), statements_(std::move(statements)) {}

ExpressionNode *RepeatStatementNode::get_expr() {
    return condition_.get();
}

StatementSequenceNode *RepeatStatementNode::get_statements() {
    return statements_.get();
};
