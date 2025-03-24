//
// Class and enums to handle info on a variable's type (used both in semantic checking and in code generation)
// Created by M on 10.03.2025.
//

#ifndef OBERON0C_TYPEINFO_HPP
#define OBERON0C_TYPEINFO_HPP

#include <string>
#include <vector>
#include <variant>
#include <map>
#include <llvm/IR/Type.h>

// records a "general type category", i.e. whether the type refers to an integer, boolean, array, record, ...
enum TypeTag
{
    RECORD,
    ARRAY,
    POINTER,
    INTEGER,
    BOOLEAN,
    FLOAT,
    CHAR,
    STRING,
    ALIAS,
    NIL,
    ERROR_TAG
};


struct TypeInfo;

struct AliasTypeInfo{
    std::string aliased_type;
};

struct RecordTypeInfo{
    std::map<std::string, std::shared_ptr<TypeInfo>> fields;
    explicit RecordTypeInfo(std::map<std::string, std::shared_ptr<TypeInfo>>& fields) : fields(fields){};
};

struct ArrayTypeInfo{
    std::shared_ptr<TypeInfo> element_type;
    int size;
};

struct PointerTypeInfo{
    std::shared_ptr<TypeInfo> pointee_type;
};

typedef std::variant<RecordTypeInfo,ArrayTypeInfo,AliasTypeInfo,PointerTypeInfo> ext_info;

// encapsulates all necessary info on the type of an identifier
struct TypeInfo {
    std::string name;

    TypeTag tag;
    std::optional<ext_info> extended_info = std::nullopt;

    bool operator!=(TypeInfo other);
    bool operator==(TypeInfo other);

    TypeInfo(std::string  name, TypeTag tag, std::optional<ext_info> = std::nullopt);

};


#endif //OBERON0C_TYPEINFO_HPP
