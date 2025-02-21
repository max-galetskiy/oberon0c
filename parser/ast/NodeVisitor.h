/*
 * Node visitor for the abstract syntax tree used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/13/21.
 */

#ifndef OBERON0C_NODEVISITOR_H
#define OBERON0C_NODEVISITOR_H

#include "parser/ast/base_blocks/ExpressionNode.h"
#include "parser/ast/base_blocks/IdentNode.h"
#include "parser/ast/base_blocks/IntNode.h"
#include "parser/ast/base_blocks/SelectorNode.h"

#include "parser/ast/declarations/ArrayTypeNode.h"
#include "parser/ast/declarations/DeclarationsNode.h"
#include "parser/ast/declarations/ProcedureDeclarationNode.h"
#include "parser/ast/declarations/RecordTypeNode.h"

#include "parser/ast/statements/AssignmentNode.h"
#include "parser/ast/statements/IfStatementNode.h"
#include "parser/ast/statements/ProcedureCallNode.h"
#include "parser/ast/statements/RepeatStatementNode.h"
#include "parser/ast/statements/StatementSequenceNode.h"
#include "parser/ast/statements/WhileStatementNode.h"

#include "ModuleNode.h"

class NodeVisitor {

public:
    explicit NodeVisitor() = default;
    virtual ~NodeVisitor() noexcept;

    virtual void visit(ExpressionNode&) = 0;
    virtual void visit(BinaryExpressionNode&) = 0;
    virtual void visit(UnaryExpressionNode&)  = 0;
    virtual void visit(IdentSelectorExpressionNode&) = 0;

    virtual void visit(IdentNode&) = 0;
    virtual void visit(IntNode&) = 0;
    virtual void visit(SelectorNode&) = 0;

    virtual void visit(TypeNode&) = 0;
    virtual void visit(ArrayTypeNode&) = 0;
    virtual void visit(DeclarationsNode&) = 0;
    virtual void visit(ProcedureDeclarationNode&) = 0;
    virtual void visit(RecordTypeNode&) = 0;

    virtual void visit(StatementNode&) = 0;
    virtual void visit(AssignmentNode&) = 0;
    virtual void visit(IfStatementNode&) = 0;
    virtual void visit(ProcedureCallNode&) = 0;
    virtual void visit(RepeatStatementNode&) = 0;
    virtual void visit(StatementSequenceNode&) = 0;
    virtual void visit(WhileStatementNode&) = 0;

    virtual void visit(ModuleNode&) = 0;

};


#endif //OBERON0C_NODEVISITOR_H
