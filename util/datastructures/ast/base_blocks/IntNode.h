//
// Created by M on 21.11.2024.
//

#ifndef OBERON0C_INTNODE_H
#define OBERON0C_INTNODE_H

#include "ExpressionNode.h"

class IntNode : public ExpressionNode {

    private:
        long value_;

    public:

        IntNode(FilePos pos, long value) : ExpressionNode(pos, NodeType::integer), value_(value){};

        void accept(NodeVisitor &visitor) override;
        void print(std::ostream &stream) const override;

        [[nodiscard]] long get_value() const{return value_;}

};


#endif //OBERON0C_INTNODE_H
