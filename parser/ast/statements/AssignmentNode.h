//
// Created by M on 21.11.2024.
//

#ifndef OBERON0C_ASSIGNMENTNODE_H
#define OBERON0C_ASSIGNMENTNODE_H

#include "StatementNode.h"
#include <memory>

class IdentNode;
class SelectorNode;
class ExpressionNode;

class AssignmentNode : public StatementNode{
    private:
        std::unique_ptr<IdentNode> variable_;
        std::unique_ptr<SelectorNode> selector_;
        std::unique_ptr<ExpressionNode> expr_;

    public:
        AssignmentNode(FilePos pos, std::unique_ptr<IdentNode> variable,std::unique_ptr<SelectorNode> selector, std::unique_ptr<ExpressionNode> expr);

        void accept(NodeVisitor &visitor) override;
        void print(std::ostream &stream) const override;

        IdentNode* get_variable();
        SelectorNode* get_selector();
        ExpressionNode* get_expr();

};


#endif //OBERON0C_ASSIGNMENTNODE_H
