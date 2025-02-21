//
// Created by M on 19.11.2024.
//

#ifndef OBERON0C_PROCEDURECALLNODE_H
#define OBERON0C_PROCEDURECALLNODE_H

#include "parser/ast/statements/StatementNode.h"
#include "parser/ast/declarations/ProcedureDeclarationNode.h"
#include <memory>
#include <vector>

class IdentNode;
class SelectorNode;
class ExpressionNode;

class ProcedureCallNode : public StatementNode {

    private:
        std::unique_ptr<IdentNode> ident_;
        std::unique_ptr<SelectorNode> selector_;
        string procedure_name;

        ProcedureDeclarationNode* procedure_declaration_ = nullptr;

        std::unique_ptr<std::vector<std::unique_ptr<ExpressionNode>>> parameters_;

    public:

        explicit ProcedureCallNode(FilePos pos, std::unique_ptr<IdentNode> name, std::unique_ptr<SelectorNode> selector, std::unique_ptr<std::vector<std::unique_ptr<ExpressionNode>>> parameters = nullptr);
        void accept(NodeVisitor &visitor) override;
        void print(std::ostream &stream) const override;

        IdentNode* get_ident();
        SelectorNode* get_selector();

        void set_name(const string&);
        string get_name();

        std::vector<std::unique_ptr<ExpressionNode>>* get_parameters();

        void set_declaration(ProcedureDeclarationNode*);
        ProcedureDeclarationNode* get_declaration();

};


#endif //OBERON0C_PROCEDURECALLNODE_H
