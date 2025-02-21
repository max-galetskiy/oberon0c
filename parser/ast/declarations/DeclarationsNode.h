//
// Created by M on 21.11.2024.
//

#ifndef OBERON0C_DECLARATIONSNODE_H
#define OBERON0C_DECLARATIONSNODE_H

#include <vector>
#include "parser/ast/Node.h"
#include "parser/ast/statements/StatementSequenceNode.h"
#include "parser/ast/statements/StatementNode.h"

class IdentNode;
class ExpressionNode;
class TypeNode;
class ProcedureDeclarationNode;

typedef std::pair<std::unique_ptr<IdentNode>,std::unique_ptr<ExpressionNode>> id_expr_pair;
typedef std::pair<std::unique_ptr<IdentNode>,std::unique_ptr<TypeNode>> id_type_pair;
typedef std::pair<std::unique_ptr<std::vector<std::unique_ptr<IdentNode>>>,std::unique_ptr<TypeNode>> idlist_type_pair;

class DeclarationsNode : public Node {

    private:
        std::vector<id_expr_pair> const_list_;
        std::vector<id_type_pair> type_list_;
        std::vector<idlist_type_pair> var_list;

        std::vector<std::unique_ptr<ProcedureDeclarationNode>> procedure_list;

    public:

        explicit DeclarationsNode(FilePos pos) : Node(NodeType::declarations,pos){};

        void add_constant(std::unique_ptr<IdentNode> ident, std::unique_ptr<ExpressionNode> expr){const_list_.emplace_back(std::move(ident),std::move(expr));};
        void add_type(std::unique_ptr<IdentNode> ident, std::unique_ptr<TypeNode> type){type_list_.emplace_back(std::move(ident),std::move(type));};
        void add_var(std::unique_ptr<std::vector<std::unique_ptr<IdentNode>>> idents, std::unique_ptr<TypeNode> type) {var_list.emplace_back(std::move(idents), std::move(type));};
        void add_procedure(std::unique_ptr<ProcedureDeclarationNode> procedure){procedure_list.emplace_back(std::move(procedure));};

        void accept(NodeVisitor &visitor) override;
        void print(std::ostream &stream) const override;

        std::vector<std::pair<IdentNode*, ExpressionNode*>> get_constants();
        std::vector<std::pair<IdentNode*, TypeNode*>> get_typenames();
        std::vector<std::pair<std::vector<IdentNode*>, TypeNode*>> get_variables();
        std::vector<ProcedureDeclarationNode*> get_procedures();

};


#endif //OBERON0C_DECLARATIONSNODE_H
