//
// Created by M on 20.11.2024.
//

#include "ModuleNode.h"
#include "util/datastructures/ast/base_blocks/IdentNode.h"
#include "util/datastructures/ast/declarations/DeclarationsNode.h"
#include "util/datastructures/ast/declarations/ProcedureDeclarationNode.h"
#include "util/datastructures/ast/base_blocks/ExpressionNode.h"
#include "util/datastructures/ast/statements/StatementSequenceNode.h"
#include "util/datastructures/ast/statements/StatementNode.h"
#include "NodeVisitor.h"

void ModuleNode::accept(NodeVisitor &visitor)
{
    visitor.visit(*this);
}

string ModuleNode::to_string() const {
    string s = "MODULE " + module_name_begin_->to_string() + ";\n" + declarations_->to_string();

    if (statements_)
    {
        s += "BEGIN\n" + statements_->to_string();
    }

    s += "\nEND " + module_name_end_->to_string() + ".";
    return s;
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
}