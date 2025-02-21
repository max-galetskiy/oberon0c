//
// Created by M on 20.11.2024.
//

#include "ProcedureDeclarationNode.h"
#include "DeclarationsNode.h"
#include "parser/ast/base_blocks/IdentNode.h"
#include "parser/ast/statements/StatementSequenceNode.h"
#include "parser/ast/statements/StatementNode.h"
#include "parser/ast/base_blocks/ExpressionNode.h"
#include "parser/ast/NodeVisitor.h"


void ProcedureDeclarationNode::accept(NodeVisitor &visitor)
{
    visitor.visit(*this);
}

void ProcedureDeclarationNode::print(ostream &stream) const
{

    // Print Heading
    stream << "PROCEDURE " << *begin_name_;

    if (params_)
    {

        stream << "(";

        // Print all FPSections
        for(auto itr = params_->begin();itr != params_->end(); itr++){

            if(itr > params_->begin()){
                stream << "; ";
            }

            // Print Single FPSection
            if(std::get<0>(*(*itr))){
                stream << "VAR ";
            }

            // Print all Parameters of FPSection
            for(auto param_list_itr = (*std::get<1>(*(*itr))).begin(); param_list_itr != (*std::get<1>(*(*itr))).end(); param_list_itr++){

                if(param_list_itr > (*std::get<1>(*(*itr))).begin()){
                    stream << ", ";
                }

                stream << *(*param_list_itr);

            }

            stream << " : " << *(std::get<2>(*(*itr)));


        }

        stream << ")";

    }

    stream << ";\n";

    // Print Body
    stream << *declarations_;

    if (statements_)
    {
        stream << "BEGIN\n" << *statements_;
    }

    stream << "\nEND " << *end_name_;
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

ProcedureDeclarationNode::ProcedureDeclarationNode(FilePos pos, std::unique_ptr<IdentNode> begin_name,std::unique_ptr<parameters> params,std::unique_ptr<DeclarationsNode> declarations,std::unique_ptr<IdentNode> end_name,std::unique_ptr<StatementSequenceNode> statements)
 : Node(NodeType::procedure_declaration, pos), begin_name_(std::move(begin_name)), params_(std::move(params)), declarations_(std::move(declarations)), statements_(std::move(statements)), end_name_(std::move(end_name)) {}

