//
// Created by M on 21.11.2024.
//

#ifndef OBERON0C_IDENTNODE_H
#define OBERON0C_IDENTNODE_H

#include <llvm/IR/Value.h>
#include "util/datastructures/ast/declarations/TypeNode.h"
#include "util/datastructures/tables/ScopeTable.h"

class IdentNode : public TypeNode
{

private:
    const string name_;
    std::shared_ptr<TypeInfo> formal_type_;
    std::shared_ptr<TypeInfo> actual_type_;

public:
    IdentNode(FilePos pos, const string name) : TypeNode(NodeType::ident, pos), name_(name) {};

    void accept(NodeVisitor &visitor) override;
    void print(std::ostream &stream) const override;

    string get_value();

};

#endif // OBERON0C_IDENTNODE_H
