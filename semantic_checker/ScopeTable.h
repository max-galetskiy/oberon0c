//
// Manages the Scopes of a Oberon0 Program
// Created by M on 15.12.2024.
//

#ifndef OBERON0C_SCOPETABLE_H
#define OBERON0C_SCOPETABLE_H

#include <vector>
#include <memory>
#include <assert.h>
#include "SymbolTable.h"
#include "parser/ast/Node.h"

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
    std::optional<std::map<string, TypeInfo>> lookup_record(const string &record_name);
    TypeInfo *lookup_field(const string &record_name, const string &field_name);

    void insert(const string &name, Kind k, Node *node, GeneralType general_type, string type = "");
    void insert(const string &name, Kind k, Node *node, TypeInfo type);
    void insert_array_type(const string &name, Node *node, TypeInfo *element_type, int dimension);
    void insert_record(const string &record_name, std::vector<std::pair<string, TypeInfo>> fields);
};

#endif // OBERON0C_SCOPETABLE_H
