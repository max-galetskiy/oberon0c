//
// Created by M on 20.11.2024.
//

#ifndef OBERON0C_RECORDTYPENODE_H
#define OBERON0C_RECORDTYPENODE_H

#include <vector>
#include <unordered_map>
#include <map>
#include "parser/ast/declarations/TypeNode.h"
#include "semantic_checker/SymbolTable.h"

class IdentNode;

typedef std::unique_ptr<std::vector<std::unique_ptr<IdentNode>>> ident_list;

typedef std::pair<ident_list, std::unique_ptr<TypeNode>> field;

typedef std::pair<std::vector<string>, TypeNode *> raw_field;

class RecordTypeNode : public TypeNode
{

private:
    std::vector<std::unique_ptr<field>> fields_;
    std::unordered_map<string, TypeNode *> field_typenodes_;
    std::map<string, TypeInfo> field_types_actual_;

public:
    RecordTypeNode(FilePos pos, std::unique_ptr<field> first_field);

    void add_field_list(std::unique_ptr<field> field_list);

    void accept(NodeVisitor &visitor) override;
    void print(std::ostream &stream) const override;

    std::vector<raw_field> get_fields();
    std::map<string, TypeInfo> *get_field_types();

    void insert_field_types(std::map<string, TypeInfo> &field_types);
};

#endif // OBERON0C_RECORDTYPENODE_H
