#include "VariableTable.h"

void VariableTable::insert(std::string name, llvm::Value *var, std::shared_ptr<TypeInfoClass>type, bool is_pointer)
{
    variables_.back().insert({name, {var, type, is_pointer}});
}
std::tuple<llvm::Value *, std::shared_ptr<TypeInfoClass>, bool>* VariableTable::lookup(std::string name)
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
void VariableTable::beginScope()
{
    variables_.push_back(std::unordered_map<std::string, std::tuple<llvm::Value *, std::shared_ptr<TypeInfoClass>, bool>>());
}
void VariableTable::endScope()
{
    variables_.pop_back();
}

VariableTable::VariableTable()
{
    variables_ = std::vector<std::unordered_map<std::string, std::tuple<llvm::Value *, std::shared_ptr<TypeInfoClass>, bool>>>();
}
