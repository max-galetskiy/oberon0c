//
// Created by M on 20.11.2024.
//

#include "ProcedureDeclarationNode.h"
#include "DeclarationsNode.h"
#include "util/datastructures/ast/base_blocks/IdentNode.h"
#include "util/datastructures/ast/statements/StatementSequenceNode.h"
#include "util/datastructures/ast/statements/StatementNode.h"
#include "util/datastructures/ast/base_blocks/ExpressionNode.h"
#include "util/datastructures/ast/NodeVisitor.h"


void ProcedureDeclarationNode::accept(NodeVisitor &visitor)
{
    visitor.visit(*this);
}

string ProcedureDeclarationNode::to_string() const {

    // Print Heading
    string s = "PROCEDURE " + begin_name_->to_string();

    if (params_)
    {

        s += "(";

        // Print all FPSections
        for(auto itr = params_->begin();itr != params_->end(); itr++){

            if(itr > params_->begin()){
                s += "; ";
            }

            // Print Single FPSection
            if(std::get<0>(*(*itr))){
                s += "VAR ";
            }

            // Print all Parameters of FPSection
            for(auto param_list_itr = (*std::get<1>(*(*itr))).begin(); param_list_itr != (*std::get<1>(*(*itr))).end(); param_list_itr++){

                if(param_list_itr > (*std::get<1>(*(*itr))).begin()){
                    s += ", ";
                }

                s += (*param_list_itr)->to_string();

            }

            s += " : " + (std::get<2>(*(*itr)))->to_string();

        }

        s += ")";

    }

    if(return_type_node_){
        s += " : " + return_type_node_->to_string();
    }

    s += ";\n";

    // Print Body
    s += declarations_->to_string();

    if (statements_)
    {
        s += "BEGIN\n" + statements_->to_string();
    }

    s += "\nEND " + end_name_->to_string();
    return s;
}

std::pair<IdentNode *, IdentNode *> ProcedureDeclarationNode::get_names() const {
    return {begin_name_.get(),end_name_.get()};
}

parameters *ProcedureDeclarationNode::get_parameters() const {
    return params_.get();
}

DeclarationsNode *ProcedureDeclarationNode::get_declarations() const {
    return declarations_.get();
}

StatementSequenceNode *ProcedureDeclarationNode::get_statements() const {
    return statements_.get();
}

TypeNode *ProcedureDeclarationNode::get_return_type_node() const {
    return return_type_node_.get();
}

int ProcedureDeclarationNode::get_parameter_number() {
    if(!params_){
        return 0;
    }

    if(parameter_number.has_value()){
        return parameter_number.value();
    }

    int nr = 0;
    for(auto itr = params_->begin(); itr != params_->end(); itr++){
        nr += static_cast<int>(std::get<1>(**itr)->size());
    }

    parameter_number = std::optional<int>(nr);
    return nr;
}

void ProcedureDeclarationNode::set_types(TypeInfo *formal, TypeInfo *actual) {
    formal_return_type_ = formal;
    actual_return_type_ = actual;
}

TypeInfo *ProcedureDeclarationNode::get_formal_return_type() const {
    return formal_return_type_;
}

TypeInfo *ProcedureDeclarationNode::get_actual_return_type() const {
    return actual_return_type_;
}

ProcedureDeclarationNode::ProcedureDeclarationNode(FilePos pos, std::unique_ptr<IdentNode> begin_name,std::unique_ptr<parameters> params,std::unique_ptr<DeclarationsNode> declarations,std::unique_ptr<IdentNode> end_name,std::unique_ptr<StatementSequenceNode> statements,std::unique_ptr<TypeNode> return_type)
 : Node(NodeType::procedure_declaration, pos), begin_name_(std::move(begin_name)), params_(std::move(params)), declarations_(std::move(declarations)), statements_(std::move(statements)), return_type_node_(std::move(return_type)), end_name_(std::move(end_name)) {}

