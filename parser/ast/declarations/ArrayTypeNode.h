//
// Created by M on 19.11.2024.
//

#ifndef OBERON0C_ARRAYTYPENODE_H
#define OBERON0C_ARRAYTYPENODE_H

#include "TypeNode.h"
#include "../../../semantic_checker/SymbolTable.h"
#include <memory>
#include <optional>

class ExpressionNode;

class ArrayTypeNode : public TypeNode
{

private:
    std::unique_ptr<ExpressionNode> dim_;
    std::unique_ptr<TypeNode> type_;
    TypeInfo base_type_info_;
    std::optional<long> dimension_ = std::nullopt;

public:
    explicit ArrayTypeNode(FilePos pos, std::unique_ptr<ExpressionNode> dim, std::unique_ptr<TypeNode> type) : TypeNode(NodeType::array_type, pos), dim_(std::move(dim)), type_(std::move(type)) {};

    void accept(NodeVisitor &visitor) override;
    void print(std::ostream &stream) const override;

    ExpressionNode *get_dim_node();
    TypeNode *get_type_node();

    void set_dim(long value);
    void set_base_type_info(TypeInfo);
    TypeInfo get_base_type_info();
    std::optional<long> get_dim();
};

#endif // OBERON0C_ARRAYTYPENODE_H
