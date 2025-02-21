//
// Created by M on 22.11.2024.
//

#ifndef OBERON0C_TYPENODE_H
#define OBERON0C_TYPENODE_H

#include "parser/ast/Node.h"

class TypeNode : public Node {

    public:
    TypeNode(NodeType node_type, FilePos pos) : Node(node_type,pos) {};
    ~TypeNode();

    virtual void accept(NodeVisitor &visitor) override;
    virtual void print(std::ostream &stream) const = 0;

};

#endif //OBERON0C_TYPENODE_H
