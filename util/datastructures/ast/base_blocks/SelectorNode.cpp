//
// Created by M on 21.11.2024.
//

#include "SelectorNode.h"
#include "ExpressionNode.h"
#include "IdentNode.h"
#include "util/datastructures/ast/NodeVisitor.h"


void SelectorNode::accept(NodeVisitor &visitor)
{
    visitor.visit(*this);
}

string SelectorNode::to_string() const {

    string s;

    for (auto itr = selectors.begin(); itr != selectors.end(); itr++)
    {

        if (std::get<0>(*itr))
        {

            // Array Index
            s += "[" + std::get<2>(*itr)->to_string() + "]";
        }
        else
        {

            // Field
            s += "." + std::get<1>(*itr)->to_string();
        }
    }
    return s;
}

void SelectorNode::add_field(std::unique_ptr<IdentNode> ident)
{
    selectors.emplace_back(false, std::move(ident), nullptr);
    selectors_raw.emplace_back(false,std::get<1>(selectors.back()).get(), nullptr);
}

void SelectorNode::add_index(std::unique_ptr<ExpressionNode> expr)
{
    selectors.emplace_back(true, nullptr, std::move(expr));
    selectors_raw.emplace_back(true, nullptr,std::get<2>(selectors.back()).get());
}

SelectorNode::SelectorNode(FilePos pos) : Node(NodeType::selector, pos)
{
}

std::vector<raw_id_indx_tuple> *SelectorNode::get_selector() {
    if(selectors.empty()){
        return nullptr;
    }

    return &selectors_raw;
}


