//
// Manages the Scopes of a Oberon0 Program
// Created by M on 15.12.2024.
//

#ifndef OBERON0C_SCOPETABLE_H
#define OBERON0C_SCOPETABLE_H

#include <vector>
#include <memory>
#include <cassert>
#include "SymbolTable.h"
#include "util/datastructures/ast/Node.h"

class ScopeTable
{

private:
    std::vector<std::unique_ptr<SymbolTable>> scopes_;
    int current_scope = -1;

public:
    ScopeTable() = default;

    void beginScope();
    void endScope();

    IdentInfo *lookup(const string &name, bool only_current = false);
    std::optional<std::unordered_map<string, std::shared_ptr<TypeInfo>>> lookup_record(const string &record_name);
    std::shared_ptr<TypeInfo> lookup_field(const string &record_name, const string &field_name);
    std::shared_ptr<TypeInfo> lookup_type(const string& name);

    void insert(const string &name, Kind k, Node *node, std::shared_ptr<TypeInfo> type);

    std::shared_ptr<TypeInfo> insert_type(const string &type_name, TypeTag tag);
    std::shared_ptr<TypeInfo> insert_type(const string &type_name, std::shared_ptr<TypeInfo> elementType, int dim);
    std::shared_ptr<TypeInfo> insert_type(const string &type_name,std::unordered_map<string, std::shared_ptr<TypeInfo>> fields);

};


#endif // OBERON0C_SCOPETABLE_H
