//
// Created by M on 22.11.2024.
//

#ifndef OBERON0C_TYPENODE_H
#define OBERON0C_TYPENODE_H

#include "util/datastructures/ast/Node.h"
#include "util/datastructures/tables/TypeInfo.hpp"

class TypeNode : public Node {

private:
    std::shared_ptr<TypeInfo> formal_type;
    std::shared_ptr<TypeInfo> actual_type;

public:
    TypeNode(NodeType node_type, FilePos pos) : Node(node_type,pos) {};
    ~TypeNode();


    virtual void accept(NodeVisitor &visitor) override;
    virtual void print(std::ostream &stream) const = 0;

    void set_types(std::shared_ptr<TypeInfo> formal, std::shared_ptr<TypeInfo> actual);
    std::shared_ptr<TypeInfo> get_formal_type();
    std::shared_ptr<TypeInfo> get_actual_type();

};

#endif //OBERON0C_TYPENODE_H
