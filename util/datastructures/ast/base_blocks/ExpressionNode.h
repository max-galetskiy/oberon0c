//
// Created by M on 21.11.2024.
//

#ifndef OBERON0C_EXPRESSIONNODE_H
#define OBERON0C_EXPRESSIONNODE_H

#include <memory>
#include <optional>
#include "util/datastructures/ast/Node.h"
#include "scanner/Token.h"
#include "util/datastructures/ast/declarations/TypeNode.h"
#include "util/datastructures/ast/statements/ProcedureCallNode.h"
#include "util/datastructures/tables/ScopeTable.h"

enum SourceOperator {PLUS, MINUS, OR, MULT, DIV, FLOAT_DIV, MOD, AND, NEG, NOT, EQ, NEQ, LT, LEQ, GT, GEQ, IS, NO_OPERATOR, PAREN};   // For pretty printing (and possibly precedence) purposes, we consider Parentheses an operator too

class IdentNode;
class SelectorNode;

class ExpressionNode : public Node{

    protected:
        int precedence_ = -1;
        std::optional<long> value_ = std::nullopt;
        std::shared_ptr<TypeInfo> formal_type;
        std::shared_ptr<TypeInfo> actual_type;

    public:
        explicit ExpressionNode(FilePos pos, NodeType type);
        virtual void accept(NodeVisitor &visitor) = 0;
        virtual string to_string() const = 0;
        [[nodiscard]] int get_precedence() const;

        void set_value(long value);
        std::optional<long>get_value();

        void set_types(std::shared_ptr<TypeInfo> formal, std::shared_ptr<TypeInfo> actual);
        std::shared_ptr<TypeInfo> get_formal_type();
        std::shared_ptr<TypeInfo> get_actual_type();

        static SourceOperator token_to_op(TokenType);
        static string print_operator(SourceOperator op);
        static int op_to_precedence(SourceOperator op);
};

class UnaryExpressionNode : public ExpressionNode{

    private:
    SourceOperator op_;
    std::unique_ptr<ExpressionNode> expr_;

    public:
    UnaryExpressionNode(FilePos pos, std::unique_ptr<ExpressionNode> expr, SourceOperator op);
    void accept(NodeVisitor &visitor) override;
    [[nodiscard]] string to_string() const override;
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
    [[nodiscard]] string to_string() const override;

};

class IdentSelectorExpressionNode : public ExpressionNode{
    private:
    std::unique_ptr<IdentNode> ident_;
    std::unique_ptr<SelectorNode> selector_;

    public:
    IdentSelectorExpressionNode(FilePos pos, std::unique_ptr<IdentNode> ident, std::unique_ptr<SelectorNode> selector);
    void accept(NodeVisitor &visitor) override;
    [[nodiscard]] string to_string() const override;

    IdentNode* get_identifier();
    SelectorNode* get_selector();

};

// To avoid the pitfalls of multiple inheritance, we basically build a wrapper over a ProcedureCallNode here
class ProcedureCallExpressionNode : public ExpressionNode{
    private:
    std::unique_ptr<ProcedureCallNode> call_;
    public:
    ProcedureCallExpressionNode(FilePos pos, std::unique_ptr<ProcedureCallNode> call);
    void accept(NodeVisitor &visitor) override;
    [[nodiscard]] string to_string() const override;

    ProcedureCallNode* get_call();
};

#endif //OBERON0C_EXPRESSIONNODE_H
