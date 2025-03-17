//
// Created by M on 15.12.2024.
//

#ifndef OBERON0C_SEMANTICCHECKER_H
#define OBERON0C_SEMANTICCHECKER_H

#include <limits>
#include <optional>
#include <cmath>

#include "util/datastructures/tables/ScopeTable.h"
#include "util/Logger.h"
#include "util/datastructures/ast/NodeVisitor.h"

class SemanticChecker : NodeVisitor {

    private:
        ScopeTable scope_table_;
        Logger& logger_;

        // Current procedure that is being evaluated (nullopt for the "Main" function of a module)
        std::optional<string> current_procedure_;

        inline static const string int_string = "INTEGER";
        inline static const string bool_string = "BOOLEAN";
        inline static std::shared_ptr<TypeInfo> error_type = std::make_shared<TypeInfo>("<ERROR_TYPE>",ERROR_TAG);
        inline static std::shared_ptr<TypeInfo> boolean_type = std::make_shared<TypeInfo>(bool_string,BOOLEAN);
        inline static std::shared_ptr<TypeInfo> integer_type = std::make_shared<TypeInfo>(int_string,INTEGER);

        void report_unknown_identifier(FilePos pos, string id_name);

    public:
        explicit SemanticChecker(Logger& logger);

        // Base Blocks/Empty Definitions
        void visit(IntNode&) override;
        void visit(ExpressionNode&) override;
        void visit(UnaryExpressionNode&) override;
        void visit(BinaryExpressionNode&) override;
        void visit(IdentSelectorExpressionNode&) override;
        void visit(SelectorNode&) override;
        void visit(TypeNode&) override;
        void visit(ArrayTypeNode&) override;
        void visit(IdentNode&) override;
        void visit(RecordTypeNode&) override;

        // Declarations
        void visit(ModuleNode&) override;
        void visit(ProcedureDeclarationNode&) override;
        void visit(DeclarationsNode&) override;

        // Statements
        void visit(StatementSequenceNode&) override;
        void visit(StatementNode&) override;
        void visit(AssignmentNode&) override;
        void visit(IfStatementNode&) override;
        void visit(RepeatStatementNode&) override;
        void visit(WhileStatementNode&) override;
        void visit(ReturnStatementNode&) override;
        void visit(ProcedureCallNode&) override;

        // Record fields
        std::map<string,std::shared_ptr<TypeInfo>> key_value_map(RecordTypeNode&);

        // Typechecking
        std::shared_ptr<TypeInfo> create_new_type(TypeNode &type, string type_name, bool insert_into_table);
        std::shared_ptr<TypeInfo> trace_type(std::shared_ptr<TypeInfo> initial_type);

        std::shared_ptr<TypeInfo> check_selector_type(IdentSelectorExpressionNode&);
        std::shared_ptr<TypeInfo> check_selector_chain(IdentNode&, SelectorNode&);

        std::shared_ptr<TypeInfo> checkType(ExpressionNode&);
        std::optional<long> evaluate_expression(ExpressionNode&, bool suppress_errors = false);

        void validate_program(ModuleNode&);

};


#endif //OBERON0C_SEMANTICCHECKER_H
