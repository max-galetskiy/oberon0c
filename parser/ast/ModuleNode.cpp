//
// Created by M on 20.11.2024.
//

#include "ModuleNode.h"
#include "parser/ast/base_blocks/IdentNode.h"
#include "parser/ast/declarations/DeclarationsNode.h"
#include "parser/ast/declarations/ProcedureDeclarationNode.h"
#include "parser/ast/base_blocks/ExpressionNode.h"
#include "parser/ast/statements/StatementSequenceNode.h"
#include "parser/ast/statements/StatementNode.h"
#include "parser/ast/NodeVisitor.h"

void ModuleNode::accept(NodeVisitor &visitor)
{
    visitor.visit(*this);
}

void ModuleNode::print(ostream &stream) const
{
    stream << "MODULE " << *module_name_begin_ << ";\n" << *declarations_;

    if (statements_)
    {
        stream << "BEGIN\n" << *statements_;
    }

    stream << "\nEND " << *module_name_end_ << ".";
}

ModuleNode::ModuleNode(FilePos pos, std::unique_ptr<IdentNode> name_start, std::unique_ptr<DeclarationsNode> declarations, std::unique_ptr<StatementSequenceNode> statements, std::unique_ptr<IdentNode> name_end) : Node(NodeType::module, pos), module_name_begin_(std::move(name_start)), declarations_(std::move(declarations)), statements_(std::move(statements)), module_name_end_(std::move(name_end)) {}

std::pair<IdentNode *, IdentNode *> ModuleNode::get_name() {
    return {module_name_begin_.get(),module_name_end_.get()};
}

DeclarationsNode *ModuleNode::get_declarations() {
    return declarations_.get();
}

StatementSequenceNode *ModuleNode::get_statements() {
    return statements_.get();
};
