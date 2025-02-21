//
// Created by M on 21.11.2024.
//

#ifndef OBERON0C_EXPRESSIONNODE_H
#define OBERON0C_EXPRESSIONNODE_H

#include "parser/ast/Node.h"
#include "scanner/Token.h"
#include "parser/ast/declarations/TypeNode.h"
#include "semantic_checker/ScopeTable.h"
#include <memory>
#include <optional>

enum SourceOperator {PLUS, MINUS, OR, MULT, DIV, MOD, AND, NEG, NOT, EQ, NEQ, LT, LEQ, GT, GEQ, NO_OPERATOR, PAREN};   // For pretty printing (and possibly precedence) purposes, we consider Parentheses an operator too

class IdentNode;
class SelectorNode;

class ExpressionNode : public Node{

    protected:
        int precedence_ = -1;
        std::optional<long> value_ = std::nullopt;
        TypeInfo formal_type;
        TypeInfo actual_type;
        TypeNode* type_node = nullptr;

    public:
        explicit ExpressionNode(FilePos pos, NodeType type);
        virtual void accept(NodeVisitor &visitor) = 0;
        virtual void print(std::ostream &stream) const = 0;
        [[nodiscard]] int get_precedence() const;

        void set_value(long value);
        std::optional<long>get_value();

        void set_types(TypeInfo formal, TypeInfo actual, TypeNode* node);
        TypeInfo get_formal_type();
        TypeInfo get_actual_type();
        TypeNode* get_type_node();

        static SourceOperator token_to_op(TokenType);
        static void print_operator(std::ostream& stream, SourceOperator op);
        static int op_to_precedence(SourceOperator op);
        friend std::ostream& operator<<(std::ostream &stream, SourceOperator op);
};

class UnaryExpressionNode : public ExpressionNode{

    private:
    SourceOperator op_;
    std::unique_ptr<ExpressionNode> expr_;

    public:
    UnaryExpressionNode(FilePos pos, std::unique_ptr<ExpressionNode> expr, SourceOperator op);
    void accept(NodeVisitor &visitor) override;
    void print(std::ostream &stream) const override;
    ExpressionNode* get_expr();
    SourceOperator get_op();

};


class BinaryExpressionNode : public ExpressionNode{

    private:
    SourceOperator op_;
    std::unique_ptr<ExpressionNode> lhs_;
    std::unique_ptr<ExpressionNode> rhs_;

    public:
    BinaryExpressionNode(FilePos pos, std::unique_ptr<ExpressionNode> lhs, SourceOperator op, std::unique_ptr<ExpressionNode> rhs);
    BinaryExpressionNode* insert_rightmost(SourceOperator op, std::unique_ptr<ExpressionNode> new_rhs);
    ExpressionNode* get_rhs();
    ExpressionNode* get_lhs();
    SourceOperator get_op();
    void accept(NodeVisitor &visitor) override;
    void print(std::ostream &stream) const override;

};

class IdentSelectorExpressionNode : public ExpressionNode{
    private:
    std::unique_ptr<IdentNode> ident_;
    std::unique_ptr<SelectorNode> selector_;

    public:
    IdentSelectorExpressionNode(FilePos pos, std::unique_ptr<IdentNode> ident, std::unique_ptr<SelectorNode> selector);
    void accept(NodeVisitor &visitor) override;
    void print(std::ostream &stream) const override;

    IdentNode* get_identifier();
    SelectorNode* get_selector();

};


#endif //OBERON0C_EXPRESSIONNODE_H
