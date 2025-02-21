//
// Created by M on 21.11.2024.
//

#ifndef OBERON0C_IDENTNODE_H
#define OBERON0C_IDENTNODE_H

#include "parser/ast/declarations/TypeNode.h"
#include "semantic_checker/ScopeTable.h"

class IdentNode : public TypeNode
{

private:
    const string name_;
    TypeInfo formal_type_;
    TypeInfo actual_type_;
    TypeNode *type_node_;
    std::optional<TypeInfo> type_embedding_;

public:
    IdentNode(FilePos pos, const string name) : TypeNode(NodeType::ident, pos), name_(name) {};

    void accept(NodeVisitor &visitor) override;
    void print(std::ostream &stream) const override;

    string get_value();
    void set_type_embedding(TypeInfo);
    std::optional<TypeInfo> get_type_embedding();
    void set_types(TypeInfo formal, TypeInfo actual, TypeNode *node);
    TypeInfo get_formal_type();
    TypeInfo get_actual_type();
    TypeNode *get_type_node();
};

#endif // OBERON0C_IDENTNODE_H
