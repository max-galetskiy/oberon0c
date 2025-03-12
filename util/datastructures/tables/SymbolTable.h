//
// Manages all Identifiers within a scope of a Oberon0 Program
// Created by M on 15.12.2024.
//

#ifndef OBERON0C_SYMBOLTABLE_H
#define OBERON0C_SYMBOLTABLE_H

#include <vector>
#include <utility>
#include <unordered_map>
#include <map>
#include <optional>

#include "util/datastructures/ast/Node.h"
#include "util/datastructures/tables/TypeInfo.hpp"

// records the "kind" of an identifier, i.e. whether the identifier refers to a variable, a constant, a type or a procedure
enum Kind
{
    PROCEDURE,
    CONSTANT,
    VARIABLE,
    TYPENAME,
    ERROR_KIND
};

/*
 *   --> Kind = What is this identifier used as (Procedure, Constant, Variable, Name of a type)
 *   --> Node in AST
 *              Kind        |   Stored Node
 *              ------------------------------------------
 *              Procedure   |   ProcedureDeclarationNode
 *              Constant    |   ExpressionNode
 *              Variable    |   TypeNode
 *              Typename    |   TypeNode
 *
 *   --> Type = Type of this variable (nullptr for procedures)
 */
struct IdentInfo
{
    string name; // Useful since sometimes the name of the identifier may be "lost" along the way, e.g. when tracing
    Kind kind;
    Node *node;
    std::shared_ptr<TypeInfo> type;
};

class SymbolTable
{

private:
    std::unordered_map<string, IdentInfo> identifiers_;
    std::unordered_map<string, std::shared_ptr<TypeInfo>> types_;

public:
    explicit SymbolTable() = default;

    void insert(const string &name, Kind k, Node *node, std::shared_ptr<TypeInfo> type);

    std::shared_ptr<TypeInfo> insert_type(const string &type_name, TypeTag tag);
    std::shared_ptr<TypeInfo> insert_type(const string &type_name, const string &aliased_type);
    std::shared_ptr<TypeInfo> insert_type(const string &type_name, std::shared_ptr<TypeInfo> elementType, int dim);
    std::shared_ptr<TypeInfo> insert_type(const string &type_name,std::map<string, std::shared_ptr<TypeInfo>> fields);

    IdentInfo *lookup(const std::string &name);
    std::shared_ptr<TypeInfo> lookup_field(const string &record_name, const string &field_name);
    std::shared_ptr<TypeInfo> lookup_type(const string& name);
    std::optional<std::map<string, std::shared_ptr<TypeInfo>>> lookup_record(const string &record_name);
};

#endif // OBERON0C_SYMBOLTABLE_H
