/*
 * Base class of all AST nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#include "Node.h"

Node::~Node() = default;

NodeType Node::getNodeType() const {
    return nodeType_;
}

FilePos Node::pos() const {
    return pos_;
}

std::ostream& operator<<(std::ostream &stream, const Node &node) {
    node.print(stream);
    return stream;
}