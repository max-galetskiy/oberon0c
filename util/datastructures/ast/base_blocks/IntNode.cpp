//
// Created by M on 21.11.2024.
//

#include "IntNode.h"
#include "util/datastructures/ast/NodeVisitor.h"


void IntNode::accept(NodeVisitor &visitor)
{
    visitor.visit(*this);
}

string IntNode::to_string() const {
    return std::to_string(value_);
}
