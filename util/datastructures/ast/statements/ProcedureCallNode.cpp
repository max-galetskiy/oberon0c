//
// Created by M on 19.11.2024.
//

#include "ProcedureCallNode.h"
#include "util/datastructures/ast/base_blocks/IdentNode.h"
#include "util/datastructures/ast/base_blocks/ExpressionNode.h"
#include "util/datastructures/ast/base_blocks/SelectorNode.h"
#include "util/datastructures/ast/NodeVisitor.h"

void ProcedureCallNode::accept(NodeVisitor &visitor)
{
    visitor.visit(*this);
}

string ProcedureCallNode::to_string() const
{
    string s = ident_->to_string();

    if(selector_){
        s += selector_->to_string();
    }

    if (parameters_)
    {
        s += "(";

        for (auto itr = parameters_->begin(); itr != parameters_->end(); itr++)
        {
            if(itr > parameters_->begin()){
                s += ", ";
            }

            s += (*itr)->to_string();
        }

        s += ")";
    }
    return s;
}

ProcedureCallNode::ProcedureCallNode(FilePos pos, std::unique_ptr<IdentNode> name,std::unique_ptr<SelectorNode> selector, std::unique_ptr<std::vector<std::unique_ptr<ExpressionNode>>> parameters) : StatementNode(NodeType::procedure_call, pos), ident_(std::move(name)), selector_(std::move(selector)), parameters_(std::move(parameters))  {}

IdentNode *ProcedureCallNode::get_ident() {
    return ident_.get();
}

SelectorNode *ProcedureCallNode::get_selector() {
    return selector_.get();
}

std::vector<std::unique_ptr<ExpressionNode>> *ProcedureCallNode::get_parameters() {
    return parameters_.get();
}

void ProcedureCallNode::set_name(const string &name) {
    procedure_name = name;
}

string ProcedureCallNode::get_name() {
    return procedure_name;
}

void ProcedureCallNode::set_declaration(ProcedureDeclarationNode *declaration) {
    procedure_declaration_ = declaration;
}

ProcedureDeclarationNode *ProcedureCallNode::get_declaration() {
    return procedure_declaration_;
}

