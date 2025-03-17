//
// Created by M on 21.11.2024.
//

#ifndef OBERON0C_LITERALNODES_H
#define OBERON0C_LITERALNODES_H

#include "ExpressionNode.h"

class IntNode : public ExpressionNode {

    private:
        long value_;

    public:

        IntNode(FilePos pos, long value) : ExpressionNode(pos, NodeType::integer), value_(value){};

        void accept(NodeVisitor &visitor) override;
        [[nodiscard]] string to_string() const override;
        [[nodiscard]] long get_value() const{return value_;}

};

class BoolNode : public ExpressionNode {

    private:
        bool value_;
    public:

        BoolNode(FilePos pos, bool value) : ExpressionNode(pos,NodeType::boolean), value_(value){};

        void accept(NodeVisitor &visitor) override;
        [[nodiscard]] string to_string() const override;
        [[nodiscard]] bool get_value() const{return value_;}

};


#endif //OBERON0C_LITERALNODES_H
