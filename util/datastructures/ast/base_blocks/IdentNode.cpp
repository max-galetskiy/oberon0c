//
// Created by M on 21.11.2024.
//

#include "IdentNode.h"
#include "util/datastructures/ast/NodeVisitor.h"

void IdentNode::accept(NodeVisitor &visitor)
{
    visitor.visit(*this);
}

string IdentNode::get_value() {
    return name_;
}

string IdentNode::to_string() const {
    return name_;
}
