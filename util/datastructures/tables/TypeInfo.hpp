//
// Class and enums to handle info on a variable's type (used both in semantic checking and in code generation)
// Created by M on 10.03.2025.
//

#ifndef OBERON0C_TYPEINFO_HPP
#define OBERON0C_TYPEINFO_HPP

#include <string>
#include <vector>
#include <variant>
#include <llvm/IR/Type.h>

// records a "general type category", i.e. whether the type refers to an integer, boolean, array, record, ...
enum TypeTag
{
    RECORD,
    ARRAY,
    INTEGER,
    BOOLEAN,
    ALIAS,
    ERROR_TAG
};


struct TypeInfo;

struct AliasTypeInfo{
    std::string aliased_type;
};

struct RecordTypeInfo{
    std::unordered_map<std::string, std::shared_ptr<TypeInfo>> fields;
    RecordTypeInfo(std::unordered_map<std::string, std::shared_ptr<TypeInfo>>& fields) : fields(fields){};
};

struct ArrayTypeInfo{
    std::shared_ptr<TypeInfo> elementType;
    int size;
};

typedef std::variant<RecordTypeInfo,ArrayTypeInfo,AliasTypeInfo> ext_info;

// encapsulates all necessary info on the type of an identifier
struct TypeInfo {
    std::string name;

    TypeTag tag;
    std::vector<llvm::Type*> llvmType;
    std::optional<ext_info> extended_info = std::nullopt;

    bool operator!=(TypeInfo other);
    bool operator==(TypeInfo other);

    TypeInfo(std::string  name, TypeTag tag, std::optional<ext_info> = std::nullopt, std::vector<llvm::Type*> = {});

};


#endif //OBERON0C_TYPEINFO_HPP
