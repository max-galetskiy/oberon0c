//
// Created by M on 20.11.2024.
//

#include "RecordTypeNode.h"
#include "util/datastructures/ast/base_blocks/IdentNode.h"
#include "util/datastructures/ast/NodeVisitor.h"

void RecordTypeNode::accept(NodeVisitor &visitor)
{
    visitor.visit(*this);
}

string RecordTypeNode::to_string() const {

    string s = "RECORD ";

    for (auto itr = fields_.begin(); itr != fields_.end(); itr++)
    {

        if (itr > fields_.begin())
        {
            s += "; ";
        }

        // Print single Field List
        auto curr_field_list = itr->get()->first.get();
        for(auto field_itr = curr_field_list->begin(); field_itr != curr_field_list->end(); field_itr++){

            if(field_itr > curr_field_list->begin()){
                s += ", ";
            }

            s += (*field_itr)->to_string();

        }

        s += " : " + (itr->get()->second)->to_string();

    }

    s += " END\n";
    return s;
}

RecordTypeNode::RecordTypeNode(FilePos pos, std::unique_ptr<field > first_field) : TypeNode(NodeType::record_type, pos) { add_field_list(std::move(first_field)); }

void RecordTypeNode::add_field_list(std::unique_ptr<field> field_list) {

    auto identifiers = field_list->first.get();
    auto type = field_list->second.get();

    for(auto itr = identifiers->begin(); itr != identifiers->end(); itr++){

        field_typenodes_[itr->get()->get_value()] = type;

    }

    fields_.emplace_back(std::move(field_list));


}

std::vector<raw_field> RecordTypeNode::get_fields() {
    std::vector<raw_field> fields;

    for(auto itr = fields_.begin(); itr != fields_.end(); itr++){

        std::vector<string> id_names;
        for(auto idents = itr->get()->first->begin(); idents != itr->get()->first->end(); idents++){
            id_names.emplace_back(idents->get()->get_value());
        }

        fields.emplace_back(id_names,itr->get()->second.get());

    }

    return fields;
}

void RecordTypeNode::insert_field_types(std::map<string, std::shared_ptr<TypeInfo>>& field_types) {
    field_typeinfos_ = field_types;
}

std::map<string, std::shared_ptr<TypeInfo>> *RecordTypeNode::get_field_types() {
    return &field_typeinfos_;
}