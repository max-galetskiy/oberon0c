#include "TypeNode.h"

#include <utility>
#include "util/datastructures/ast/NodeVisitor.h"

TypeNode::~TypeNode() = default;

void TypeNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void TypeNode::set_types(std::shared_ptr<TypeInfo> formal, std::shared_ptr<TypeInfo> actual) {
    formal_type = std::move(formal);
    actual_type = std::move(actual);
}

std::shared_ptr<TypeInfo> TypeNode::get_formal_type() {
    return formal_type;
}

std::shared_ptr<TypeInfo> TypeNode::get_actual_type() {
    return actual_type;
}
