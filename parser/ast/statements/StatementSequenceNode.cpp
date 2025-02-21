//
// Created by M on 18.11.2024.
//

#include "StatementSequenceNode.h"
#include "StatementNode.h"
#include "parser/ast/NodeVisitor.h"

void StatementSequenceNode::accept(NodeVisitor &visitor)
{
    visitor.visit(*this);
}

void StatementSequenceNode::print(ostream &stream) const
{
    for (auto itr = statements_.begin(); itr != statements_.end(); itr++)
    {

        if (itr > statements_.begin())
        {
            stream << ";\n";
        }

        stream << "\t" << *(*itr);
    }
}

StatementSequenceNode::StatementSequenceNode(FilePos pos, std::unique_ptr<StatementNode> first_statement) : Node(NodeType::statement_sequence, pos) { statements_.emplace_back(std::move(first_statement)); }

void StatementSequenceNode::add_statement(std::unique_ptr<StatementNode> statement) { statements_.emplace_back(std::move(statement)); }

std::vector<std::unique_ptr<StatementNode>> *StatementSequenceNode::get_statements() {
    return &statements_;
};
