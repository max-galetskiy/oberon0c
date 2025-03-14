//
// Created by M on 20.11.2024.
//

#ifndef OBERON0C_PROCEDUREDECLARATIONNODE_H
#define OBERON0C_PROCEDUREDECLARATIONNODE_H

#include <vector>
#include <optional>
#include "util/datastructures/ast/Node.h"
#include "util/datastructures/tables/TypeInfo.hpp"

class IdentNode;
class TypeNode;
class DeclarationsNode;
class StatementSequenceNode;

typedef std::tuple<bool,std::unique_ptr<std::vector<std::unique_ptr<IdentNode>>>,std::unique_ptr<TypeNode>> fp_section_t;

typedef std::vector<std::unique_ptr<fp_section_t>> parameters;

class ProcedureDeclarationNode : public Node {

    private:
        std::unique_ptr<IdentNode> begin_name_;
        std::unique_ptr<parameters> params_;

        std::unique_ptr<DeclarationsNode> declarations_;
        std::unique_ptr<TypeNode> return_type_node_;
        std::unique_ptr<StatementSequenceNode> statements_;
        std::unique_ptr<IdentNode> end_name_;

        TypeInfo* formal_return_type_;
        TypeInfo* actual_return_type_;

        std::optional<int> parameter_number;

    public:
        ProcedureDeclarationNode(FilePos pos, std::unique_ptr<IdentNode> name, std::unique_ptr<parameters> params, std::unique_ptr<DeclarationsNode> declarations, std::unique_ptr<IdentNode> end_name, std::unique_ptr<StatementSequenceNode> statements = nullptr, std::unique_ptr<TypeNode> return_type = nullptr);

        void accept(NodeVisitor &visitor) override;
        string to_string() const override;

        void set_types(TypeInfo* formal, TypeInfo* actual);

        [[nodiscard]] std::pair<IdentNode*,IdentNode*> get_names() const;
        [[nodiscard]] parameters* get_parameters() const;
        [[nodiscard]] DeclarationsNode* get_declarations() const;
        [[nodiscard]] StatementSequenceNode* get_statements() const;
        [[nodiscard]] TypeNode* get_return_type_node() const;
        [[nodiscard]] TypeInfo* get_formal_return_type() const;
        [[nodiscard]] TypeInfo* get_actual_return_type() const;
        int get_parameter_number();

};


#endif //OBERON0C_PROCEDUREDECLARATIONNODE_H
