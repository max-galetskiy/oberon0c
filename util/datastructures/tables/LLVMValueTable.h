#include <vector>
#include <unordered_map>
#include <utility>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>

#ifndef OBERON0_VARIABLETABLE_H
#define OBERON0_VARIABLETABLE_H

struct VariableInfo {
    llvm::Value* value;
    llvm::Type* type;
    bool is_pointer;
};

class LLVMValueTable
{
private:
    std::vector<std::unordered_map<std::string, VariableInfo>> variables_;
    std::vector<std::unordered_map<std::string, llvm::Type*>>  types_;

public:
    LLVMValueTable();

    void insert(std::string, llvm::Value *, llvm::Type*, bool is_pointer = false);
    void insert_type(std::string, llvm::Type*);

    VariableInfo* lookup(std::string);
    llvm::Type* lookup_type(std::string);

    void beginScope();
    void endScope();
};

#endif // OBERON0_VARIABLETABLE_H