//
// Created by M on 14.03.2025.
//

#include "ReturnStatementNode.hpp"
#include "util/datastructures/ast/base_blocks/ExpressionNode.h"
#include "util/datastructures/ast/NodeVisitor.h"

ReturnStatementNode::ReturnStatementNode(FilePos &pos, std::unique_ptr<ExpressionNode> value) : StatementNode(NodeType::return_statement,pos), value_(std::move(value)) {}

string ReturnStatementNode::to_string() const {
    string s = "RETURN";

    if(value_){
        s += " " + value_->to_string();
    }

    return s;
}

void ReturnStatementNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

ExpressionNode *ReturnStatementNode::get_value() {
    return value_.get();
}