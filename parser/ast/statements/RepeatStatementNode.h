//
// Created by M on 18.11.2024.
//

#ifndef OBERON0C_REPEATSTATEMENT_H
#define OBERON0C_REPEATSTATEMENT_H

#include "parser/ast/statements/StatementNode.h"
#include <memory>

class ExpressionNode;
class StatementSequenceNode;

class RepeatStatementNode : public StatementNode {

    private:
        std::unique_ptr<ExpressionNode> condition_;
        std::unique_ptr<StatementSequenceNode> statements_;

    public:
        RepeatStatementNode(FilePos pos, std::unique_ptr<ExpressionNode> condition, std::unique_ptr<StatementSequenceNode> statements);
        void print(std::ostream &stream) const override;
        void accept(NodeVisitor &visitor) override;

        ExpressionNode* get_expr();
        StatementSequenceNode* get_statements();
};


#endif //OBERON0C_REPEATSTATEMENT_H
