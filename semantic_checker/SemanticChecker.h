//
// Created by M on 15.12.2024.
//

#ifndef OBERON0C_SEMANTICCHECKER_H
#define OBERON0C_SEMANTICCHECKER_H

#include "ScopeTable.h"
#include "util/Logger.h"
#include "parser/ast/NodeVisitor.h"
#include <limits>
#include <optional>
#include <cmath>


class SemanticChecker : NodeVisitor {

    private:
        ScopeTable scope_table_;
        Logger& logger_;

        inline static const string int_string = "INTEGER";
        inline static const string bool_string = "BOOLEAN";
        inline static const TypeInfo error_type = {ERROR_TYPE, ""};
        inline static const TypeInfo boolean_type = {BOOLEAN, bool_string};
        inline static const TypeInfo integer_type = {INTEGER, int_string};


    public:
        explicit SemanticChecker(Logger& logger);

        // Base Blocks/Empty Definitions
        void visit(IntNode&) override;
        void visit(ExpressionNode&) override;
        void visit(UnaryExpressionNode&) override;
        void visit(BinaryExpressionNode&) override;
        void visit(IdentSelectorExpressionNode&) override;
        void visit(SelectorNode&) override;

        // Declarations
        void visit(ModuleNode&) override;
        void visit(ProcedureDeclarationNode&) override;
        void visit(DeclarationsNode&) override;

        // Type Nodes
        void visit(TypeNode&) override;
        void visit(ArrayTypeNode&) override;
        void visit(IdentNode&) override;
        void visit(RecordTypeNode&) override;

        // Statements
        void visit(StatementSequenceNode&) override;
        void visit(StatementNode&) override;
        void visit(AssignmentNode&) override;
        void visit(IfStatementNode&) override;
        void visit(RepeatStatementNode&) override;
        void visit(WhileStatementNode&) override;
        void visit(ProcedureCallNode&) override;

        // Record fields
        std::vector<std::pair<string,TypeInfo>> key_value_map(RecordTypeNode&);

        // Typechecking
        TypeInfo get_type(TypeNode&, const string&);
        TypeInfo trace_type(TypeInfo initial_type);

        TypeInfo check_selector_type(IdentSelectorExpressionNode&);
        TypeInfo check_selector_chain(IdentNode&, SelectorNode&);

        TypeInfo checkType(ExpressionNode&);
        std::optional<long> evaluate_expression(ExpressionNode&, bool suppress_errors = false);

        void validate_program(ModuleNode&);

};


#endif //OBERON0C_SEMANTICCHECKER_H
