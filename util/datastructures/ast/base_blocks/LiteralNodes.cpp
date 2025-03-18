//
// Created by M on 21.11.2024.
//

#include "LiteralNodes.h"
#include "util/datastructures/ast/NodeVisitor.h"


void IntNode::accept(NodeVisitor &visitor)
{
    visitor.visit(*this);
}

string IntNode::to_string() const {
    return std::to_string(value_);
}

void BoolNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

string BoolNode::to_string() const {
    return (value_)? "TRUE" : "FALSE";
}

void FloatNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

string FloatNode::to_string() const {
    return std::to_string(value_);
}
