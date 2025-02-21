//
// Created by M on 18.11.2024.
//

#ifndef OBERON0C_STATEMENTSEQUENCENODE_H
#define OBERON0C_STATEMENTSEQUENCENODE_H

#include <vector>
#include "parser/ast/Node.h"
#include <memory>

class StatementNode;

class StatementSequenceNode : public Node{

    private:
        std::vector<std::unique_ptr<StatementNode>> statements_;

    public:
        StatementSequenceNode(FilePos pos,std::unique_ptr<StatementNode> first_statement);
        void add_statement(std::unique_ptr<StatementNode> statement);
        void accept(NodeVisitor &visitor) override;
        void print(std::ostream &stream) const override;

        std::vector<std::unique_ptr<StatementNode>>* get_statements();
};


#endif //OBERON0C_STATEMENTSEQUENCENODE_H
