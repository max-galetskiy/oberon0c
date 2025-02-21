#include "TypeInfoTable.h"

void TypeInfoTable::insert(std::string name, TypeInfoClass type_info)
{
    types_.back().insert({name, type_info});
}

TypeInfoClass *TypeInfoTable::lookup(std::string name)
{

    for (auto it = types_.rbegin(); it != types_.rend(); ++it)
    {
        if (it->find(name) != it->end())
        {
            return &(*it)[name];
        }
    }

    return nullptr;
}

void TypeInfoTable::beginScope()
{
    types_.push_back(std::unordered_map<std::string, TypeInfoClass>());
}
void TypeInfoTable::endScope()
{
    types_.pop_back();
}