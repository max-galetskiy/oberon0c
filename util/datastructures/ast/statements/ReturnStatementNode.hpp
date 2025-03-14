//
// Created by M on 14.03.2025.
//

#ifndef OBERON0C_RETURNSTATEMENTNODE_HPP
#define OBERON0C_RETURNSTATEMENTNODE_HPP

#include "StatementNode.h"

class ExpressionNode;

class ReturnStatementNode : public StatementNode{
    private:
        std::unique_ptr<ExpressionNode> value_;

    public:
        explicit ReturnStatementNode(FilePos& pos, std::unique_ptr<ExpressionNode> value = nullptr);
        string to_string() const override;
        void accept(NodeVisitor &visitor) override;

        ExpressionNode* get_value();
};


#endif //OBERON0C_RETURNSTATEMENTNODE_HPP
