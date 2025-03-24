//
// Created by M on 22.03.2025.
//

#ifndef OBERON0C_POINTERTYPENODE_HPP
#define OBERON0C_POINTERTYPENODE_HPP

#include "TypeNode.h"
#include "util/datastructures/tables/TypeInfo.hpp"

class PointerTypeNode : public TypeNode {

    private:
        std::unique_ptr<TypeNode> pointee_type_;

    public:
        PointerTypeNode(FilePos& pos, std::unique_ptr<TypeNode> pointee_type) : TypeNode(NodeType::pointer_type,pos), pointee_type_(std::move(pointee_type)){};

        void accept(NodeVisitor&) override;
        [[nodiscard]] string to_string() const override;

        TypeNode* get_pointee_typenode();
};


#endif //OBERON0C_POINTERTYPENODE_HPP
