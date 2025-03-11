//
// Created by M on 15.12.2024.
//

#include "SymbolTable.h"

#include <utility>

void SymbolTable::insert(const std::string &name, Kind k, Node *node, std::shared_ptr<TypeInfo> type)
{

    // Check if already inserted
    if (identifiers_.find(name) != identifiers_.end())
    {
        return;
    }

    identifiers_[name] = IdentInfo(name, k, node, type);
}

IdentInfo *SymbolTable::lookup(const std::string &name)
{

    auto node = identifiers_.find(name);
    return (node != identifiers_.end()) ? &node->second : nullptr;
}

std::shared_ptr<TypeInfo> SymbolTable::lookup_field(const string &record_name, const string &field_name)
{
    auto record_itr = types_.find(record_name);
    if (record_itr == types_.end())
    {
        return nullptr;
    }

    auto record_type = types_[record_name];
    if(record_type->tag != RECORD){
        return nullptr;
    }

    auto field_map = get<RecordTypeInfo>(record_type->extended_info.value()).fields;
    auto field = field_map.find(field_name);

    if (field == field_map.end())
    {
        return nullptr;
    }

    return (*field).second;
}

std::optional<std::unordered_map<string, std::shared_ptr<TypeInfo>>> SymbolTable::lookup_record(const string &record_name) {
    auto record = types_.find(record_name);
    if(record == types_.end()){
        return std::nullopt;
    }

    if(record->second->tag != RECORD){
        return std::nullopt;
    }

    return get<RecordTypeInfo>(record->second->extended_info.value()).fields;
}

std::shared_ptr<TypeInfo> SymbolTable::insert_type(const string &type_name, TypeTag tag) {
    types_[type_name] = std::make_shared<TypeInfo>(type_name,tag);
    return types_[type_name];
}

// Overload for ArrayTypes
std::shared_ptr<TypeInfo> SymbolTable::insert_type(const string &type_name, std::shared_ptr<TypeInfo> elementType, int dim) {
    types_[type_name] = std::make_shared<TypeInfo>(type_name,ARRAY,ArrayTypeInfo(std::move(elementType),dim));
    return types_[type_name];
}

// Overload for RecordTypes
std::shared_ptr<TypeInfo> SymbolTable::insert_type(const string &type_name,std::unordered_map<string, std::shared_ptr<TypeInfo>> fields)
{
    types_[type_name] = std::make_shared<TypeInfo>(type_name,RECORD,RecordTypeInfo(fields));
    return types_[type_name];

}

std::shared_ptr<TypeInfo> SymbolTable::lookup_type(const string &name) {
    if(types_.find(name) != types_.end()){
        return types_[name];
    }

    return nullptr;
}
