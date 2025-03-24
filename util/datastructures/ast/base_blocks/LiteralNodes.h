//
// Created by M on 21.11.2024.
//

#ifndef OBERON0C_LITERALNODES_H
#define OBERON0C_LITERALNODES_H

#include <utility>

#include "ExpressionNode.h"

class IntNode : public ExpressionNode {

    private:
        long value_;

    public:

        IntNode(FilePos pos, long value) : ExpressionNode(std::move(pos), NodeType::integer), value_(value){};

        void accept(NodeVisitor &visitor) override;
        [[nodiscard]] string to_string() const override;
        [[nodiscard]] long get_value() const{return value_;}

};

class BoolNode : public ExpressionNode {

    private:
        bool value_;
    public:

        BoolNode(FilePos pos, bool value) : ExpressionNode(std::move(pos),NodeType::boolean), value_(value){};

        void accept(NodeVisitor &visitor) override;
        [[nodiscard]] string to_string() const override;
        [[nodiscard]] bool get_value() const{return value_;}

};

class FloatNode : public ExpressionNode{

    private:
        double value_;  // kind of goes against the name, I know
    public:

    FloatNode(FilePos pos, double value) : ExpressionNode(std::move(pos),NodeType::real), value_(value){};

    void accept(NodeVisitor &visitor) override;
    [[nodiscard]] string to_string() const override;
    [[nodiscard]] double get_value() const{return value_;}

};

class CharNode : public ExpressionNode{

    private:
        unsigned char value_;
    public:

        CharNode(FilePos pos, unsigned char value) : ExpressionNode(std::move(pos),NodeType::character), value_(value){};

        void accept(NodeVisitor &visitor) override;
        [[nodiscard]] string to_string() const override;
        [[nodiscard]] unsigned char get_value() const{return value_;}

};

class StringNode : public ExpressionNode{
    private:
        string value_;
    public:

        StringNode(FilePos pos, string value) : ExpressionNode(std::move(pos),NodeType::string), value_(std::move(value)){};

        void accept(NodeVisitor &visitor) override;
        [[nodiscard]] string to_string() const override;
        [[nodiscard]] string get_value() const{return value_;}

};

class NilNode : public ExpressionNode{
    public:
        NilNode(FilePos pos) : ExpressionNode(std::move(pos),NodeType::nil){};
        void accept(NodeVisitor &visitor) override;
        [[nodiscard]] string to_string() const override;
};

#endif //OBERON0C_LITERALNODES_H
