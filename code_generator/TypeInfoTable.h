#include <vector>
#include <utility>
#include <string>
#include <variant>
#include <unordered_map>
#include <llvm/IR/Type.h>

#ifndef OBERON0C_TYPEINFOTABLE_H
#define OBERON0C_TYPEINFOTABLE_H

enum TypeTag
{
    RECORD_TAG,
    ARRAY_TAG,
    INTEGER_TAG,
    BOOLEAN_TAG,
};

struct TypeInfoClass
{
    TypeTag tag;
    std::vector<llvm::Type *> llvmType;
    struct Record
    {
        std::vector<std::pair<std::string, std::shared_ptr<TypeInfoClass>>> fields;
    };
    struct Array
    {
        std::shared_ptr<TypeInfoClass> elementType;
        int size;
    };
    std::variant<Record, Array> value;
};

class TypeInfoTable
{
private:
    std::vector<std::unordered_map<std::string, TypeInfoClass>> types_;

public:
    void insert(std::string name, TypeInfoClass type_info);
    TypeInfoClass *lookup(std::string name);

    void beginScope();
    void endScope();
};

#endif //OBERON0_TYPEINFOTAG_H