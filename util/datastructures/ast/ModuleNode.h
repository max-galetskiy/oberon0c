//
// Created by M on 20.11.2024.
//

#ifndef OBERON0C_MODULENODE_H
#define OBERON0C_MODULENODE_H

#include "Node.h"

class IdentNode;
class DeclarationsNode;
class StatementSequenceNode;

class ModuleNode : public Node {

    private:
    std::unique_ptr<IdentNode> module_name_begin_;
    std::unique_ptr<DeclarationsNode> declarations_;
    std::unique_ptr<StatementSequenceNode> statements_;
    std::unique_ptr<IdentNode> module_name_end_;

    public:

    ModuleNode(FilePos pos, std::unique_ptr<IdentNode> name_start, std::unique_ptr<DeclarationsNode> declarations, std::unique_ptr<StatementSequenceNode> statements, std::unique_ptr<IdentNode> name_end);


    void accept(NodeVisitor &visitor) override;
    void print(std::ostream &stream) const override;

    std::pair<IdentNode*,IdentNode*> get_name();
    DeclarationsNode* get_declarations();
    StatementSequenceNode* get_statements();

};


#endif //OBERON0C_MODULENODE_H
