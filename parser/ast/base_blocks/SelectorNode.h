//
// Created by M on 21.11.2024.
//

#ifndef OBERON0C_SELECTORNODE_H
#define OBERON0C_SELECTORNODE_H

#include "parser/ast/Node.h"
#include <vector>
#include <memory>

class IdentNode;
class ExpressionNode;

typedef std::tuple<bool,std::unique_ptr<IdentNode>,std::unique_ptr<ExpressionNode>> id_indx_tuple;
typedef std::tuple<bool,IdentNode*,ExpressionNode*> raw_id_indx_tuple;

class SelectorNode : public Node {

    private:
        std::vector<id_indx_tuple> selectors;
        std::vector<raw_id_indx_tuple> selectors_raw;

    public:
        explicit SelectorNode(FilePos pos);

        void add_field(std::unique_ptr<IdentNode> ident);
        void add_index(std::unique_ptr<ExpressionNode> expr);

        void accept(NodeVisitor &visitor) override;
        void print(std::ostream &stream) const override;

        std::vector<raw_id_indx_tuple>* get_selector();

};


#endif //OBERON0C_SELECTORNODE_H
