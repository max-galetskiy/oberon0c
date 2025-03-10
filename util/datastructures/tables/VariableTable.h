#include <vector>
#include <unordered_map>
#include <utility>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>
#include "TypeInfoTable.h"

#ifndef OBERON0_VARIABLETABLE_H
#define OBERON0_VARIABLETABLE_H

class VariableTable
{
private:
    std::vector<std::unordered_map<std::string, std::tuple<llvm::Value *, std::shared_ptr<TypeInfoClass>, bool >>> variables_;

public:
    VariableTable(/* args */);
    void insert(std::string, llvm::Value *, std::shared_ptr<TypeInfoClass>, bool is_pointer = false);
    std::tuple<llvm::Value *, std::shared_ptr<TypeInfoClass>, bool>* lookup(std::string);
    void beginScope();
    void endScope();
};

#endif // OBERON0_VARIABLETABLE_H