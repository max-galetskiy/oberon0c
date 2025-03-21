//
// Created by M on 10.03.2025.
//

#include "TypeInfo.hpp"

#include <utility>

bool TypeInfo::operator!=(TypeInfo other) {
    return !(*this == std::move(other));
}

bool TypeInfo::operator==(TypeInfo other) {

    switch(this->tag){
        case BOOLEAN:
        case INTEGER:
        case FLOAT:
        case CHAR:
        case STRING:
            return (this->tag == other.tag);
        case ARRAY:
            return(other.tag == ARRAY &&
                  (std::get<ArrayTypeInfo>(this->extended_info.value())).size == (std::get<ArrayTypeInfo>(other.extended_info.value())).size &&
                  (std::get<ArrayTypeInfo>(this->extended_info.value())).elementType == (std::get<ArrayTypeInfo>(other.extended_info.value())).elementType);
        case ALIAS:
            return(other.tag == ALIAS && this->name == other.name);
        case RECORD: // Note that it never really makes sense to compare two different record types
        case ERROR_TAG:
        default:
            return false;
    }
}

TypeInfo::TypeInfo(std::string name, TypeTag tag, std::optional<ext_info> val) : name(std::move(name)),tag(tag), extended_info(val){}
