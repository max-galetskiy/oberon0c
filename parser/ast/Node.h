/*
 * Base class of all AST nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#ifndef OBERON0C_AST_H
#define OBERON0C_AST_H


#include <list>
#include <string>
#include <ostream>
#include <utility>
#include "../../util/Logger.h"
#include <memory>


enum class NodeType : char {
    array_type,
    assignment,
    declarations,
    unary_expression,
    binary_expression,
    ident_selector_expression,
    ident,
    if_statement,
    integer,
    module,
    procedure_call,
    procedure_declaration,
    record_type,
    repeat_statement,
    selector,
    statement,
    statement_sequence,
    type,
    while_statement
};

class NodeVisitor;

class Node {

private:
    NodeType nodeType_;
    FilePos pos_;

public:
    explicit Node(const NodeType nodeType, FilePos pos) : nodeType_(nodeType), pos_(std::move(pos)) { };
    virtual ~Node();

    [[nodiscard]] NodeType getNodeType() const;
    [[nodiscard]] FilePos pos() const;

    virtual void accept(NodeVisitor &visitor) = 0;

    virtual void print(std::ostream &stream) const = 0;
    friend std::ostream& operator<<(std::ostream &stream, const Node &node);

};

#endif //OBERON0C_AST_H