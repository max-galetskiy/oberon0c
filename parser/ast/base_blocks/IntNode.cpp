//
// Created by M on 21.11.2024.
//

#include "IntNode.h"
#include "parser/ast/NodeVisitor.h"


void IntNode::accept(NodeVisitor &visitor)
{
    visitor.visit(*this);
}

void IntNode::print(ostream &stream) const
{
    stream << value_;
}
