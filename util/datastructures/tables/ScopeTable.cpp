//
// Created by M on 15.12.2024.
//

#include "ScopeTable.h"

#include <utility>

void ScopeTable::beginScope()
{
    current_scope++;
    scopes_.emplace_back(std::make_unique<SymbolTable>());
}

void ScopeTable::endScope()
{
    current_scope--;
    scopes_.pop_back();
}

IdentInfo *ScopeTable::lookup(const string &name, bool only_current)
{

    if (only_current)
    {
        assert(current_scope >= 0);

        return scopes_[static_cast<size_t>(current_scope)]->lookup(name);
    }

    for (int i = current_scope; i >= 0; i--)
    {

        auto el = scopes_[static_cast<size_t>(i)]->lookup(name);
        if (el)
        {
            return el;
        }
    }

    return nullptr;
}

void ScopeTable::insert(const std::string &name, Kind k, Node *node, std::shared_ptr<TypeInfo> type)
{
    assert(current_scope >= 0);
    scopes_[static_cast<size_t>(current_scope)]->insert(name, k, node, type);
}

std::shared_ptr<TypeInfo> ScopeTable::lookup_field(const string &record_name, const string &field_name)
{

    for (int i = current_scope; i >= 0; i--)
    {

        auto rec = scopes_[static_cast<size_t>(i)]->lookup_field(record_name, field_name);
        if (rec)
        {
            return rec;
        }
    }

    return nullptr;
}

std::optional<std::unordered_map<string, std::shared_ptr<TypeInfo>>> ScopeTable::lookup_record(const string &record_name) {
    for (int i = current_scope; i >= 0; i--)
    {
        auto rec = scopes_[static_cast<size_t>(i)]->lookup_record(record_name);
        if (rec)
        {
            return rec;
        }
    }

    return std::nullopt;
}

std::shared_ptr<TypeInfo> ScopeTable::lookup_type(const string &name) {

    for(int i = current_scope; i >= 0; i--){
        auto type = scopes_[static_cast<size_t>(i)]->lookup_type(name);
        if(type){
            return type;
        }
    }

    return nullptr;
}

std::shared_ptr<TypeInfo> ScopeTable::insert_type(const string &type_name, TypeTag tag) {
    assert(current_scope >= 0);
    return scopes_[static_cast<size_t>(current_scope)]->insert_type(type_name,tag);
}

std::shared_ptr<TypeInfo> ScopeTable::insert_type(const string &type_name, std::shared_ptr<TypeInfo> elementType, int dim) {
    assert(current_scope >= 0);
    return scopes_[static_cast<size_t>(current_scope)]->insert_type(type_name,std::move(elementType),dim);
}

std::shared_ptr<TypeInfo> ScopeTable::insert_type(const string &type_name, std::unordered_map<string, std::shared_ptr<TypeInfo>> fields) {
    return scopes_[static_cast<size_t>(current_scope)]->insert_type(type_name,std::move(fields));
}



