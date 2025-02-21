//
// Created by M on 15.12.2024.
//

#include "SymbolTable.h"

#include <utility>

void SymbolTable::insert(const std::string &name, Kind k, Node *node, GeneralType general_type, string type)
{

    // Check if already inserted
    if (table_.find(name) != table_.end())
    {
        return;
    }

    table_[name] = IdentInfo(name, k, node, {general_type,std::move(type)});
}

void SymbolTable::insert(const string &name, Kind k, Node *node, const TypeInfo& type) {
    if(table_.find(name) != table_.end()){
        return;
    }

    table_[name] = IdentInfo(name,k, node,{type.general,type.name,type.array_dim,type.element_type});
}

void SymbolTable::insert_array_type(const string &name, Node *node, TypeInfo *element_type, int dimension) {
    if(table_.find(name) != table_.end()){
        return;
    }

    table_[name] = IdentInfo(name, Kind::TYPENAME, node, {ARRAY,name,dimension,std::make_shared<TypeInfo>(*element_type)});
}

IdentInfo *SymbolTable::lookup(const std::string &name)
{

    // return nullptr if not included
    auto node = table_.find(name);
    return (node != table_.end()) ? &node->second : nullptr;
}

void SymbolTable::insert_record(const string &record_name, std::vector<std::pair<string, TypeInfo>> fields)
{
    std::map<string, TypeInfo> field_map;
    for (auto itr = fields.begin(); itr != fields.end(); itr++)
    {
        field_map[itr->first] = itr->second;
    }

    records_[record_name] = field_map;

}

TypeInfo* SymbolTable::lookup_field(const string &record_name, const string &field_name)
{
    auto record = records_.find(record_name);
    if (record == records_.end())
    {
        return nullptr;
    }

    auto field = records_[record_name].find(field_name);
    if (field == records_[record_name].end())
    {
        return nullptr;
    }

    return &field->second;
}

std::optional<std::map<string, TypeInfo>> SymbolTable::lookup_record(const string &record_name) {
    auto record = records_.find(record_name);
    if(record == records_.end()){
        return std::nullopt;
    }

    return records_[record_name];
}


bool TypeInfo::operator!=(TypeInfo other) {
    return !(*this == std::move(other));
}

bool TypeInfo::operator==(TypeInfo other) {

    switch(this->general){
        case BOOLEAN:
        case INTEGER:
            return (this->general == other.general);
        case ARRAY:
            return(other.general == ARRAY && this->array_dim==other.array_dim && this->element_type == other.element_type);
        case ALIAS:
            return(other.general == ALIAS && this->name == other.name);
        case RECORD:
        case ERROR_TYPE:
        default:
            return false;
    }
}
