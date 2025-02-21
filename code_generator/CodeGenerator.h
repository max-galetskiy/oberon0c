//
// Created by M on 19.01.2025.
//

#ifndef OBERON0C_CODEGENERATOR_H
#define OBERON0C_CODEGENERATOR_H

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <vector>
#include <unordered_map>
#include <utility>
#include "TypeInfoTable.h"
#include "VariableTable.h"
#include "parser/ast/NodeVisitor.h"

enum class OutputFileType
{
    AssemblyFile,
    LLVMIRFile,
    ObjectFile
};

using namespace llvm;

class CodeGenerator : public NodeVisitor
{

private:
    TargetMachine *target_;
    Module *module_;
    LLVMContext ctx_;
    OutputFileType output_type_;
    const string filename_;
    IRBuilder<> *builder_;

    std::unordered_map<string, Function *> procedures_;

    llvm::Value *value_;
    VariableTable variables_;
    TypeInfoTable type_table_;
    TypeInfoClass temp_type_;

    void init_target_machine();
    void init_builder();
    void emit();

public:
    CodeGenerator(string filename, OutputFileType output_type);

    void visit(ExpressionNode &) override;
    void visit(BinaryExpressionNode &) override;
    void visit(UnaryExpressionNode &) override;
    void visit(IdentSelectorExpressionNode &) override;
    void LoadIdentSelector(IdentNode &ident, SelectorNode *selector, bool return_pointer = false);
    void LoadIdent(IdentNode &, bool return_pointer = false);

    void visit(IdentNode &) override;
    void visit(IntNode &) override;
    [[noreturn]] void visit(SelectorNode &) override;

    void visit(TypeNode &) override;
    void visit(ArrayTypeNode &) override;
    void visit(DeclarationsNode &) override;
    void create_declarations(DeclarationsNode &node, bool is_global = false);
    void visit(ProcedureDeclarationNode &) override;
    void visit(RecordTypeNode &) override;

    void visit(StatementNode &) override;
    void visit(AssignmentNode &) override;
    void visit(IfStatementNode &) override;
    void visit(ProcedureCallNode &) override;
    void visit(RepeatStatementNode &) override;
    void visit(StatementSequenceNode &) override;
    void visit(WhileStatementNode &) override;

    void visit(ModuleNode &) override;

    void generate_code(ModuleNode &);
};

#endif // OBERON0C_CODEGENERATOR_H
