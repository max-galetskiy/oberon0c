#include "TypeNode.h"
#include "util/datastructures/ast/NodeVisitor.h"

TypeNode::~TypeNode() = default;

void TypeNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}
