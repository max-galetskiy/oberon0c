//
// Created by M on 22.03.2025.
//

#include "PointerTypeNode.hpp"
#include "util/datastructures/ast/NodeVisitor.h"

void PointerTypeNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

string PointerTypeNode::to_string() const {
    return "POINTER TO " + pointee_type_->to_string();
}

TypeNode *PointerTypeNode::get_pointee_typenode() {
    return pointee_type_.get();
}
