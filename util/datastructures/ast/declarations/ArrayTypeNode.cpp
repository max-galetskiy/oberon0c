//
// Created by M on 19.11.2024.
//

#include "ArrayTypeNode.h"
#include "util/datastructures/ast/base_blocks/ExpressionNode.h"
#include "util/datastructures/ast/NodeVisitor.h"

void ArrayTypeNode::accept(NodeVisitor &visitor)
{
    visitor.visit(*this);
}

void ArrayTypeNode::print(ostream &stream) const
{
    stream << "ARRAY " << *dim_ << " OF " << *type_;
}

ExpressionNode *ArrayTypeNode::get_dim_node()
{
    return dim_.get();
}

TypeNode *ArrayTypeNode::get_type_node()
{
    return type_.get();
}

void ArrayTypeNode::set_dim(long value)
{
    dimension_ = std::optional<long>(value);
}

std::optional<long> ArrayTypeNode::get_dim()
{
    return dimension_;
}
