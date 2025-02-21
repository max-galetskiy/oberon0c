#include "TypeNode.h"
#include "parser/ast/NodeVisitor.h"

TypeNode::~TypeNode() = default;

void TypeNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}
