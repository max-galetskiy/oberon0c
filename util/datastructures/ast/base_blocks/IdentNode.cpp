//
// Created by M on 21.11.2024.
//

#include "IdentNode.h"
#include "util/datastructures/ast/NodeVisitor.h"

void IdentNode::accept(NodeVisitor &visitor)
{
    visitor.visit(*this);
}

void IdentNode::print(ostream &stream) const
{
    stream << name_;
}

string IdentNode::get_value() {
    return name_;
}
