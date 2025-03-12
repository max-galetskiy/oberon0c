#include "LLVMValueTable.h"

void LLVMValueTable::insert(std::string name, llvm::Value *var, bool is_pointer)
{
    variables_.back()[name] = {var,is_pointer};
}

void LLVMValueTable::insert_type(std::string name, llvm::Type *type) {
    types_.back()[name] = type;
}

VariableInfo* LLVMValueTable::lookup(std::string name)
{
    for (auto it = variables_.rbegin(); it != variables_.rend(); ++it)
    {
        if (it->find(name) != it->end())
        {
            return &(*it)[name];
        }
    }

    return nullptr;
}

llvm::Type *LLVMValueTable::lookup_type(std::string name) {
    for (auto it = types_.rbegin(); it != types_.rend(); ++it)
    {
        if (it->find(name) != it->end())
        {
            return (*it)[name];
        }
    }

    return nullptr;
}

void LLVMValueTable::beginScope()
{
    variables_.push_back(std::unordered_map<std::string, VariableInfo>());
    types_.push_back(std::unordered_map<std::string,llvm::Type*>());
}

void LLVMValueTable::endScope()
{
    variables_.pop_back();
    types_.pop_back();
}

LLVMValueTable::LLVMValueTable()
{

}