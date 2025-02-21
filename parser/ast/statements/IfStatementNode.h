//
// Created by M on 18.11.2024.
//

#ifndef OBERON0C_IFSTATEMENTNODE_H
#define OBERON0C_IFSTATEMENTNODE_H

#include <vector>
#include "StatementNode.h"

class ExpressionNode;
class StatementSequenceNode;

typedef std::pair<std::unique_ptr<ExpressionNode>, std::unique_ptr<StatementSequenceNode>> ElseIfPair;

class IfStatementNode : public StatementNode {

    private:
        std::unique_ptr<ExpressionNode> condition_;
        std::unique_ptr<StatementSequenceNode> then_statements_;
        std::vector<ElseIfPair> else_ifs_ = {};
        std::unique_ptr<StatementSequenceNode> else_statements_ = nullptr;

    public:

        IfStatementNode(FilePos pos, std::unique_ptr<ExpressionNode> condition, std::unique_ptr<StatementSequenceNode> then_statements);

        void add_else_if(std::unique_ptr<ExpressionNode> expr, std::unique_ptr<StatementSequenceNode> statements);
        void add_else(std::unique_ptr<StatementSequenceNode> else_statements);

        void accept(NodeVisitor &visitor) override;
        void print(std::ostream &stream) const override;

        ExpressionNode* get_condition();
        StatementSequenceNode* get_then();
        std::vector<ElseIfPair>* get_else_ifs();
        StatementSequenceNode* get_else();
};


#endif //OBERON0C_IFSTATEMENTNODE_H
