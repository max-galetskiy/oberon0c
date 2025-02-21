//
// Created by M on 15.12.2024.
//

#include "SemanticChecker.h"

SemanticChecker::SemanticChecker(Logger &logger) : logger_(logger)
{
    scope_table_ = ScopeTable();
}

// Expressions:
//      --> Use of invalid types must be reported
//      --> Expressions must be type-checked

// Type Checking
//      --> Arithmetic Operators must have INTEGER values and return INTEGERs
//      --> Comparison Operators must have the same type and  return BOOLEANs
//      --> Boolean Operators must have BOOLEAN values and return BOOLEANs
// Type is returned as a string with special types denotes by an underscore (as Oberon0 does not allow underscores as the start of identifiers)
TypeInfo SemanticChecker::checkType(ExpressionNode &expr)
{

    auto type = expr.getNodeType();

    if (type == NodeType::binary_expression)
    {
        auto bin_expr = &dynamic_cast<BinaryExpressionNode &>(expr);

        auto lhs = bin_expr->get_lhs();
        auto rhs = bin_expr->get_rhs();
        auto op = bin_expr->get_op();

        // Comparison Operators
        if (op == SourceOperator::EQ || op == SourceOperator::NEQ ||
            op == SourceOperator::LEQ || op == SourceOperator::GEQ ||
            op == SourceOperator::LT || op == SourceOperator::GT)
        {

            auto l_type = checkType(*lhs);
            auto r_type = checkType(*rhs);

            if (trace_type(l_type).general == RECORD ||
                trace_type(r_type).general == RECORD ||
                trace_type(l_type).general == ARRAY ||
                trace_type(r_type).general == ARRAY)
            {
                logger_.error(expr.pos(), "Illegal use of comparison operators with array/record types.");
                return error_type;
            }
            if (l_type.general != r_type.general || l_type.name != r_type.name)
            {
                logger_.error(expr.pos(), "LHS and RHS of Boolean expression do not have equal types.");
                return error_type;
            }

            return boolean_type;
        }

        // Arithmetic Operators
        else if (op == SourceOperator::PLUS || op == SourceOperator::MINUS ||
                 op == SourceOperator::MULT || op == SourceOperator::DIV ||
                 op == SourceOperator::MOD)
        {

            if (trace_type(checkType(*lhs)).general != INTEGER)
            {
                logger_.error(expr.pos(), "LHS of arithmetic expression is not of type INTEGER.");
                return error_type;
            }
            if (trace_type(checkType(*rhs)).general != INTEGER)
            {
                logger_.error(expr.pos(), "RHS of arithmetic expression is not of type INTEGER.");
                return error_type;
            }

            expr.set_types(integer_type, integer_type, nullptr);
            return integer_type;
        }

        // Boolean Operators
        else if (op == SourceOperator::AND || op == SourceOperator::OR)
        {

            if (checkType(*lhs).general != BOOLEAN)
            {
                logger_.error(expr.pos(), "LHS of Boolean expression is not of type BOOLEAN.");
                return error_type;
            }
            if (checkType(*rhs).general != BOOLEAN)
            {
                logger_.error(expr.pos(), "RHS of Boolean expression is not of type BOOLEAN.");
                return error_type;
            }

            expr.set_types(boolean_type, boolean_type, nullptr);
            return boolean_type;
        }
    }

    else if (type == NodeType::unary_expression)
    {

        auto un_expr = &dynamic_cast<UnaryExpressionNode &>(expr);
        auto op = un_expr->get_op();
        auto inner = un_expr->get_expr();

        if (op == SourceOperator::NOT)
        {
            if (checkType(*inner).general != BOOLEAN)
            {
                logger_.error(expr.pos(), "Cannot negate an expression that's not of type BOOLEAN.");
                return error_type;
            }

            expr.set_types(boolean_type, boolean_type, nullptr);
            return boolean_type;
        }
        else if (op == SourceOperator::NEG)
        {
            if (trace_type(checkType(*inner)).general != INTEGER)
            {
                logger_.error(expr.pos(), "Expression is not of type INTEGER.");
                return error_type;
            }
            expr.set_types(boolean_type, boolean_type, nullptr);
            return integer_type;
        }
        else if (op == SourceOperator::NO_OPERATOR || op == SourceOperator::PAREN)
        {
            auto inner_type = checkType(*inner);
            expr.set_types(inner_type, trace_type(inner_type), nullptr);
            return inner_type;
        }
    }
    else if (type == NodeType::ident_selector_expression)
    {

        auto id_expr = &dynamic_cast<IdentSelectorExpressionNode &>(expr);
        auto id_selector_type = check_selector_type(*id_expr);
        expr.set_types(id_selector_type, trace_type(id_selector_type), nullptr);
        return id_selector_type;
    }
    else if (type == NodeType::integer)
    {
        expr.set_types(integer_type, integer_type, nullptr);
        return integer_type;
    }
    else
    {
        logger_.error(expr.pos(), "Invalid or empty expression.");
        return error_type;
    }

    logger_.error(expr.pos(), "Could not deduce expression type.");
    return error_type;
}

// Checks if expression is constant and if so evaluates it (returns nullopt in cases where errors occur)
std::optional<long> SemanticChecker::evaluate_expression(ExpressionNode &expr, bool suppress_errors)
{

    auto type = expr.getNodeType();

    if (type == NodeType::binary_expression)
    {
        auto bin_expr = &dynamic_cast<BinaryExpressionNode &>(expr);

        auto lhs = evaluate_expression(*bin_expr->get_lhs(), suppress_errors);
        auto rhs = evaluate_expression(*bin_expr->get_rhs(), suppress_errors);
        auto op = bin_expr->get_op();

        if (!lhs || !rhs)
        {
            return std::nullopt;
        }

        switch (op)
        {
        case SourceOperator::MINUS:
            return lhs.value() - rhs.value();
        case SourceOperator::MOD:
            return (lhs.value() % rhs.value());
        case SourceOperator::DIV:
            if (rhs.value() == 0)
            {
                logger_.error(expr.pos(), "Division by zero.");
                return std::nullopt;
            }
            return lhs.value() / rhs.value();
        case SourceOperator::MULT:
            return lhs.value() * rhs.value();
        case SourceOperator::PLUS:
            return lhs.value() + rhs.value();
        default:
            if (!suppress_errors)
            {
                logger_.error(expr.pos(), "Could not evaluate expression to an integer.");
            }
            return std::nullopt;
        }
    }
    else if (type == NodeType::unary_expression)
    {

        auto un_expr = &dynamic_cast<UnaryExpressionNode &>(expr);
        auto op = un_expr->get_op();
        auto inner = evaluate_expression(*un_expr->get_expr(), suppress_errors);

        if (!inner)
        {
            return std::nullopt;
        }

        if (op == SourceOperator::NEG)
        {
            return -inner.value();
        }
        else if (op == SourceOperator::NO_OPERATOR || op == SourceOperator::PAREN)
        {
            return inner.value();
        }
    }
    else if (type == NodeType::ident_selector_expression)
    {

        // Can only be evaluated if the expression consists of only a constant identifier
        auto id_sel_expr = &dynamic_cast<IdentSelectorExpressionNode &>(expr);

        if (id_sel_expr->get_selector() && !id_sel_expr->get_selector()->get_selector())
        {
            if (!suppress_errors)
            {
                logger_.error(expr.pos(), "Constant expression contains non-constant elements (array-indexing or record-fields).");
            }
            return std::nullopt;
        }

        auto id_info = scope_table_.lookup(id_sel_expr->get_identifier()->get_value());
        if (!id_info)
        {
            if (!suppress_errors)
            {
                logger_.error(expr.pos(), "Use of unknown identifier: '" + id_sel_expr->get_identifier()->get_value() + "'.");
            }
            return std::nullopt;
        }

        if (id_info->kind != Kind::CONSTANT)
        {
            if (!suppress_errors)
            {
                logger_.error(expr.pos(), "Constant expression contains non-constant identifiers.");
            }
            return std::nullopt;
        }

        // Get value of constant
        if (!id_info->node)
        {
            if (!suppress_errors)
            {
                logger_.error(expr.pos(), "Could not find value of the constant: '" + id_sel_expr->get_identifier()->get_value() + "'.");
            }
            return std::nullopt;
        }

        auto constant_expr = dynamic_cast<const ExpressionNode *>(id_info->node);
        return evaluate_expression(const_cast<ExpressionNode &>(*constant_expr), suppress_errors);
    }
    else if (type == NodeType::integer)
    {
        auto integer_node = &dynamic_cast<IntNode &>(expr);
        return integer_node->get_value();
    }

    if (!suppress_errors)
    {
        logger_.error(expr.pos(), "Could not evaluate expression to an integer.");
    }
    return std::nullopt;
}

// Type inference for an ident-selector expression
TypeInfo SemanticChecker::check_selector_type(IdentSelectorExpressionNode &id_expr)
{
    auto identifier = id_expr.get_identifier();
    auto selector = id_expr.get_selector();

    // check if identifier is defined
    auto identifier_info = scope_table_.lookup(identifier->get_value());
    if (!identifier_info)
    {
        logger_.error(id_expr.pos(), "Unknown Identifier: " + identifier->get_value());
        return error_type;
    }

    if (!selector || !selector->get_selector())
    {
        return identifier_info->type;
    }

    // Check the selector chain for validity (visit the selector)
    return check_selector_chain(*identifier, *selector);
}

// Traces back types to handle type-aliases like the following:
// TYPE INTARRAY = ARRAY 20 OF INTEGER
// TYPE INA      = INTARRAY
// TYPE ABC      = INA
TypeInfo SemanticChecker::trace_type(TypeInfo initial_type)
{

    if (initial_type.general != ALIAS)
    {
        return initial_type;
    }

    TypeInfo current_type = initial_type;
    IdentInfo *curr_info;

    while ((curr_info = scope_table_.lookup(initial_type.name)) && (current_type.general == ALIAS))
    {

        // Easy exit should this function be called with an erroneous type (which happens for semantically incorrect programs)
        if (curr_info->kind != Kind::TYPENAME)
        {
            return error_type;
        }

        current_type = curr_info->type;
    }

    return current_type;
}

// Selector:
//      --> Validity has to be checked through the entire chain
//      --> For Array-Indexing:
//                  * Indexed object must actually have an array-type
//                  * Index must have the type INTEGER
//                  * If Expression can be evaluated, Array bounds need to be checked
//      --> For Field-Selection:
//                  * Object must actually have a record type
//                  * Identifier must refer to an actual field of that record type
TypeInfo SemanticChecker::check_selector_chain(IdentNode &ident, SelectorNode &selector)
{
    IdentInfo *prev_info = scope_table_.lookup(ident.get_value());

    if (!selector.get_selector())
    {
        return prev_info->type;
    }

    auto chain = selector.get_selector();

    // Traverse entire chain
    // Note that the validity of the first selector cannot be checked inside this function
    for (auto itr = chain->begin(); itr != chain->end(); itr++)
    {

        if (std::get<0>(*itr))
        {

            // Array Index

            // Object must be an array type
            if (trace_type(prev_info->type).general != ARRAY)
            {
                logger_.error(selector.pos(), "Tried to index non-array object.");
                return error_type;
            }

            // Index must evaluate to a non-negative integer
            auto expr = std::get<2>(*itr);
            if (trace_type(checkType(*expr)).general != INTEGER)
            {
                logger_.error(selector.pos(), "Array index does not evaluate to an integer.");
                return error_type;
            }

            TypeInfo arr_type = trace_type(prev_info->type);
            int dim = arr_type.array_dim;

            // If the expression can be evaluated, array bounds need to be checked
            auto evaluated_dim = evaluate_expression(*expr, true);
            if (evaluated_dim)
            {
                expr->set_value(evaluated_dim.value());
                if (evaluated_dim.value() > dim)
                {
                    logger_.error(selector.pos(), "Array index out of bounds (index " + to_string(evaluated_dim.value()) + " for size " + to_string(dim) + ").");
                }
            }

            // Update prev_info
            TypeInfo elem_type = *arr_type.element_type;

            if (elem_type.general == INTEGER)
            {
                prev_info = scope_table_.lookup("INTEGER");
            }
            else if (elem_type.general == ALIAS)
            {
                prev_info = scope_table_.lookup(elem_type.name);
            }

            if (!prev_info)
            {
                logger_.error(selector.pos(), "Unable to get information for type '" + elem_type.name + "'");
                return error_type;
            }
        }
        else
        {

            // Record Field

            // Object must actually have a record type
            if (trace_type(prev_info->type).general != RECORD)
            {
                logger_.error(selector.pos(), "Tried to access field of a non-record object.");
                return error_type;
            }

            // Identifier must refer to an actual field of that record type
            string record_name = scope_table_.lookup(prev_info->type.name)->name;
            TypeInfo *field_type = scope_table_.lookup_field(record_name, std::get<1>(*itr)->get_value());
            if (!field_type || field_type->general == ERROR_TYPE)
            {
                logger_.error(selector.pos(), "Tried to access invalid field of record type '" + record_name + "' (Field: " + std::get<1>(*itr)->get_value() + ").");
                return error_type;
            }

            // Update prev_info
            prev_info = scope_table_.lookup(field_type->name);

            if (!prev_info)
            {
                logger_.error(selector.pos(), "Unable to get information for type '" + field_type->name + "'");
                return error_type;
            }
        }
    }

    return prev_info->type;
}

// Converts TypeNode to Type
TypeInfo SemanticChecker::get_type(TypeNode &type, const string &alias = "")
{

    if (type.getNodeType() == NodeType::ident)
    {

        auto ident_node = dynamic_cast<IdentNode &>(type);

        const string ident_name = ident_node.get_value();

        if (ident_name == int_string)
        {
            ident_node.set_type_embedding(integer_type);
            return integer_type;
        }
        else if (ident_name == bool_string)
        {
            ident_node.set_type_embedding(boolean_type);
            return boolean_type;
        }
        else
        {
            ident_node.set_type_embedding({ALIAS, ident_name});
            return {ALIAS, ident_name};
        }
    }
    else if (type.getNodeType() == NodeType::array_type)
    {
        auto array_node = &dynamic_cast<ArrayTypeNode &>(type);
        auto dim = evaluate_expression(*array_node->get_dim_node());

        if (!dim)
        {
            return error_type;
        }

        array_node->set_dim(dim.value());

        auto elem_type = get_type(*array_node->get_type_node());
        array_node->set_base_type_info(elem_type);

        return {ARRAY, alias.empty() ? "ARRAY" : alias, static_cast<int>(dim.value()), std::make_shared<TypeInfo>(elem_type)};
    }
    else if (type.getNodeType() == NodeType::record_type)
    {
        return {RECORD, alias.empty() ? "RECORD" : alias};
    }

    std::cerr << "Invalid NodeType passed as TypeNode!" << std::endl;
    return error_type;
}

// Module:
//      --> The beginning and ending names should align
//      --> A module opens a new scope
void SemanticChecker::visit(ModuleNode &module)
{

    scope_table_.beginScope();

    // Insert pre-defined types "INTEGER" and "BOOLEAN"
    scope_table_.insert("INTEGER", Kind::TYPENAME, nullptr, integer_type);
    scope_table_.insert("BOOLEAN", Kind::TYPENAME, nullptr, boolean_type);

    auto names = module.get_name();

    if (names.first->get_value() != names.second->get_value())
    {
        logger_.error(module.pos(), "Beginning and ending names of module are unequal: \"" + names.first->get_value() + "\" and \"" + names.second->get_value() + "\".");
    }

    // validate declarations
    visit(*module.get_declarations());

    // validate statements
    visit(*module.get_statements());

    scope_table_.endScope();
}

// Procedure Declaration:
//      --> Beginning and ending names should align
//      --> A procedure opens a new scope
void SemanticChecker::visit(ProcedureDeclarationNode &procedure)
{

    auto names = procedure.get_names();
    if (names.first->get_value() != names.second->get_value())
    {
        logger_.error(procedure.pos(), "Beginning and ending names of procedure are unequal: \"" + names.first->get_value() + "\" and \"" + names.second->get_value() + "\".");
    }

    // Check for double declarations
    if (scope_table_.lookup(names.first->get_value(), true))
    {
        logger_.error(procedure.pos(), "Multiple declarations for procedure '" + names.first->get_value() + "' found (Note: Oberon0 does not allow overloading functions).");
    }

    // Save the procedure name (before opening up a new scope!)
    scope_table_.insert(names.first->get_value(), Kind::PROCEDURE, &procedure, ERROR_TYPE);

    // Open up new scope
    scope_table_.beginScope();

    // Check the parameters
    auto params = procedure.get_parameters();

    if (params)
    {
        for (auto itr = params->begin(); itr != params->end(); itr++)
        {

            // Check Type definition
            TypeNode *type = std::get<2>(**itr).get();
            TypeInfo var_type;
            if (type->getNodeType() == NodeType::array_type || type->getNodeType() == NodeType::record_type)
            {
                var_type = get_type(*type);
                logger_.error(type->pos(), "New Type defined in formal parameters of function. Actual Parameter will never be able to fulfill this type (Note: The Oberon0 compiler follows name-equivalence, not structural equivalence).");
            }
            else
            {

                // Corresponding Type has to be looked up
                auto type_info = scope_table_.lookup(dynamic_cast<IdentNode *>(type)->get_value());
                var_type = (type_info) ? type_info->type : error_type;
            }
            visit(*type);

            for (auto var = std::get<1>(**itr)->begin(); var != std::get<1>(**itr)->end(); var++)
            {

                // Check for double definitions
                if (scope_table_.lookup(var->get()->get_value(), true))
                {
                    logger_.error(var->get()->pos(), "Multiple use of the same parameter name.");
                }

                var->get()->set_types(var_type, trace_type(var_type), type);
                scope_table_.insert(var->get()->get_value(), Kind::VARIABLE, var->get(), var_type);
            }
        }
    }

    // Check declarations
    visit(*procedure.get_declarations());

    // Check statements
    visit(*procedure.get_statements());

    scope_table_.endScope();
}

// Declarations:
//      --> Every identifier should be stored in the scope table with its corresponding information
//      --> Erroneous declarations/definitions should be reported
void SemanticChecker::visit(DeclarationsNode &declars)
{

    auto constants = declars.get_constants();
    auto variables = declars.get_variables();
    auto typenames = declars.get_typenames();
    auto procedures = declars.get_procedures();

    // Constants:
    //      --> Expression must evaluate to a constant
    for (auto itr = constants.begin(); itr != constants.end(); itr++)
    {

        // check for double declarations (only in current scope)
        if (scope_table_.lookup(itr->first->get_value(), true))
        {
            logger_.error(declars.pos(), "Multiple declarations of identifier '" + itr->first->get_value() + "'.");
        }

        // check if expression actually evaluates to a constant
        auto evaluated_value = evaluate_expression(*itr->second);
        if (!evaluated_value)
        {
            logger_.error(itr->second->pos(), "Right hand side of constant does not evaluate to a constant.");
        }
        else
        {
            itr->second->set_value(evaluated_value.value());
        }

        // insert variable into scope table
        auto const_type = checkType(*itr->second);
        itr->first->set_types(const_type, trace_type(const_type), nullptr);
        scope_table_.insert(itr->first->get_value(), Kind::CONSTANT, itr->second, const_type.general, const_type.name);
    }

    // Typenames
    //      --> Type definition must be valid
    for (auto itr = typenames.begin(); itr != typenames.end(); itr++)
    {

        // check for double declarations
        if (scope_table_.lookup(itr->first->get_value(), true))
        {
            logger_.error(declars.pos(), "Multiple Declarations of identifier '" + itr->first->get_value() + "'.");
        }

        // check the type definition
        visit(*itr->second);

        // insert into scope table
        auto type_def = get_type(*itr->second, itr->first->get_value());
        itr->first->set_types(type_def, trace_type(type_def), itr->second);
        scope_table_.insert(itr->first->get_value(), Kind::TYPENAME, itr->second, type_def);

        // if the type referred to a record-type, then we also store the record information in the scope table
        if (itr->second->getNodeType() == NodeType::record_type)
        {
            scope_table_.insert_record(itr->first->get_value(), key_value_map(dynamic_cast<RecordTypeNode &>(*itr->second)));

            // Insert Traced Record Types into RecordTypeNode
            auto field_map_opt = scope_table_.lookup_record(itr->first->get_value());
            if (field_map_opt)
            {
                auto field_map = field_map_opt.value();

                // Trace Types
                for (auto fitr = field_map.begin(); fitr != field_map.end(); fitr++)
                {
                    fitr->second = trace_type(fitr->second);
                }

                dynamic_cast<RecordTypeNode &>(*itr->second).insert_field_types(field_map);
            }
        }
    }

    // Variables:
    //      --> Type must be valid
    for (auto itr = variables.begin(); itr != variables.end(); itr++)
    {

        // check for valid types (i.e., visit Type Node)
        visit(*itr->second);

        TypeInfo var_type;
        TypeNode *var_typenode = nullptr;

        // Assign type (if right hand side is a variable, then the corresponding type info has to be looked up
        // Correctly Assign TypeNode
        if (itr->second->getNodeType() == NodeType::ident)
        {
            auto type_info = scope_table_.lookup(dynamic_cast<IdentNode *>(itr->second)->get_value());
            var_type = (type_info) ? type_info->type : error_type;

            if (type_info && type_info->kind == Kind::TYPENAME)
            {
                auto traced_type_info = scope_table_.lookup(trace_type(var_type).name);
                var_typenode = (traced_type_info) ? dynamic_cast<TypeNode *>(traced_type_info->node) : nullptr;
            }
        }
        else
        {
            var_type = get_type(*itr->second);
            var_typenode = itr->second;
        }

        for (auto el = itr->first.begin(); el != itr->first.end(); el++)
        {

            // check for double declarations (only in current scope)
            if (scope_table_.lookup((*el)->get_value(), true))
            {
                logger_.error(declars.pos(), "Multiple Declarations of identifier '" + (*el)->get_value() + "'.");
            }

            // insert variable into symbol table
            (*el)->set_types(var_type, trace_type(var_type), var_typenode);
            scope_table_.insert((*el)->get_value(), Kind::VARIABLE, itr->second, var_type);
        }
    }

    // Procedures
    for (auto itr = procedures.begin(); itr != procedures.end(); itr++)
    {
        visit(*(*itr));
    }
}

// Type: (Recall that Type is an abstract class)
void SemanticChecker::visit(TypeNode &node)
{
    if (node.getNodeType() == NodeType::ident)
    {
        visit(dynamic_cast<IdentNode &>(node));
    }
    else if (node.getNodeType() == NodeType::array_type)
    {
        visit(dynamic_cast<ArrayTypeNode &>(node));
    }
    else if (node.getNodeType() == NodeType::record_type)
    {
        visit(dynamic_cast<RecordTypeNode &>(node));
    }
}

// Identifier:
//      --> Must refer to a valid type (Note that this visit method is only called on typenames)
void SemanticChecker::visit(IdentNode &node)
{
    auto info = scope_table_.lookup(node.get_value());
    if (!info)
    {
        logger_.error(node.pos(), "Use of unknown identifier: '" + node.get_value() + "'.");
        return;
    }

    if (info->kind != Kind::TYPENAME)
    {
        logger_.error(node.pos(), "Identifier '" + node.get_value() + "' does not refer to a type.");
    }
}

// ArrayType:
//      --> Specified dimension must evaluate to an integer greater than zero
//      --> Specified type must exist (and be a type)
void SemanticChecker::visit(ArrayTypeNode &node)
{

    auto dim_expr = node.get_dim_node();
    auto dim = evaluate_expression(*dim_expr);

    // Check dimensions
    if (!dim.has_value())
    {
        logger_.error(dim_expr->pos(), "Specified array dimensions do not evaluate to a constant.");
    }
    else if (dim.value() <= 0)
    {
        logger_.error(dim_expr->pos(), "Cannot create array of size " + to_string(dim.value()) + ".");
    }
    else
    {
        node.set_dim(dim.value());
    }

    // Check type
    auto type = node.get_type_node();
    visit(*type);
}

// RecordType:
//      --> RecordType definition opens a new scope
//      --> All field names must be unique
//      --> Types of the fields must be valid
//      --> RecordType is inserted correctly in ScopeTable
void SemanticChecker::visit(RecordTypeNode &node)
{

    scope_table_.beginScope();

    auto fields = node.get_fields();

    for (auto field_itr = fields.begin(); field_itr != fields.end(); field_itr++)
    {

        // Check for double names
        for (auto field_name = field_itr->first.begin(); field_name != field_itr->first.end(); field_name++)
        {
            if (scope_table_.lookup(*field_name, true))
            {
                logger_.error(node.pos(), "Multiple definitions of record field '" + *field_name + "'.");
            }

            // Note: Since this scope is only used to check for double definitions and immediately closed after that,
            //       It doesn't *really* matter what we put into the scope table for the fields
            scope_table_.insert(*field_name, Kind::VARIABLE, nullptr, error_type);
        }

        // Check for valid type definition
        visit(*field_itr->second);
    }

    scope_table_.endScope();
}

// Fills a key-value-map-vector which is needed to place record types into the scope table
std::vector<std::pair<string, TypeInfo>> SemanticChecker::key_value_map(RecordTypeNode &node)
{
    std::vector<std::pair<string, TypeInfo>> key_value_map;

    auto fields = node.get_fields();
    for (auto field_itr = fields.begin(); field_itr != fields.end(); field_itr++)
    {
        for (auto field_name = field_itr->first.begin(); field_name != field_itr->first.end(); field_name++)
        {
            key_value_map.emplace_back(*field_name, get_type(*field_itr->second));
        }
    }

    return key_value_map;
}

// StatementSequence:
//      --> Each statement has to be valid
void SemanticChecker::visit(StatementSequenceNode &node)
{
    auto statements = node.get_statements();
    for (auto itr = statements->begin(); itr != statements->end(); itr++)
    {
        visit(**itr);
    }
}

// Statement: (Recall that Statement is an abstract class
void SemanticChecker::visit(StatementNode &node)
{
    if (node.getNodeType() == NodeType::assignment)
    {
        visit(dynamic_cast<AssignmentNode &>(node));
    }
    else if (node.getNodeType() == NodeType::if_statement)
    {
        visit(dynamic_cast<IfStatementNode &>(node));
    }
    else if (node.getNodeType() == NodeType::while_statement)
    {
        visit(dynamic_cast<WhileStatementNode &>(node));
    }
    else if (node.getNodeType() == NodeType::repeat_statement)
    {
        visit(dynamic_cast<RepeatStatementNode &>(node));
    }
    else if (node.getNodeType() == NodeType::procedure_call)
    {
        visit(dynamic_cast<ProcedureCallNode &>(node));
    }
}

// Assignment:
//      --> LHS must be a non-const variable
//      --> RHS must have the same type as the LHS
void SemanticChecker::visit(AssignmentNode &node)
{

    // Check LHS
    auto lhs_id = node.get_variable()->get_value();
    auto lhs_id_info = scope_table_.lookup(lhs_id);

    if (!lhs_id_info)
    {
        logger_.error(node.pos(), "Use on unknown identifier: '" + lhs_id + "'.");
        return;
    }

    if (lhs_id_info->kind == Kind::CONSTANT)
    {
        logger_.error(node.pos(), "Tried to re-assign a value to the constant '" + lhs_id + "'.");
        return;
    }

    if (lhs_id_info->kind != Kind::VARIABLE)
    {
        logger_.error(node.pos(), "Can't assign a value to a non-variable.");
        return;
    }

    // Check Selector / Get Type of Variable
    auto lhs_type = lhs_id_info->type;
    if (node.get_selector())
    {
        lhs_type = check_selector_chain(*node.get_variable(), *node.get_selector());
        if (lhs_type.general == ERROR_TYPE)
        {
            return;
        }
    }

    // Check RHS
    auto rhs = node.get_expr();
    auto expr_type = checkType(*rhs);

    if ((expr_type != lhs_type) && expr_type.general != ERROR_TYPE)
    { // We exclude the error case to avoid too many exceptions
        logger_.error(node.pos(), "Cannot assign something of type '" + expr_type.name + "' to a variable of type '" + lhs_type.name + "'.");
        return;
    }

    // If both have a record type, there needs to be more checking done
}

// If Statements:
//      --> All Expressions must evaluate to type BOOLEAN
//      --> All Statements must be valid
void SemanticChecker::visit(IfStatementNode &node)
{

    // Initial If-Then
    auto condition = node.get_condition();
    if (checkType(*condition).general != BOOLEAN)
    {
        logger_.error(condition->pos(), "Condition of If-Statement does not evaluate to a BOOLEAN.");
    }
    visit(*node.get_then());

    // Else-Ifs
    auto else_ifs = node.get_else_ifs();
    for (auto itr = else_ifs->begin(); itr != else_ifs->end(); itr++)
    {

        if (checkType(*itr->first).general != BOOLEAN)
        {
            logger_.error(itr->first->pos(), "Condition of Else-If-Statement does not evaluate to a BOOLEAN.");
        }
        visit(*itr->second);
    }

    // Else
    auto else_statement = node.get_else();
    if (else_statement)
    {
        visit(*else_statement);
    }
}

// Repeat Statement:
//      --> Condition must evaluate to a Boolean
//      --> Statements must be valid
void SemanticChecker::visit(RepeatStatementNode &node)
{

    auto condition = node.get_expr();
    if (checkType(*condition).general != BOOLEAN)
    {
        logger_.error(node.pos(), "Condition of Repeat-Statement does not evaluate to the type BOOLEAN.");
    }

    // Validate Statements
    visit(*node.get_statements());
}

// While Statement:
//      --> Condition must evaluate to a Boolean
//      --> Statements must be valid
void SemanticChecker::visit(WhileStatementNode &node)
{
    auto condition = node.get_expr();
    if (checkType(*condition).general != BOOLEAN)
    {
        logger_.error(node.pos(), "Condition of While-Loop does not evaluate to the type BOOLEAN.");
    }

    // Validate Statements
    visit(*node.get_statements());
}

// ProcedureCall:
//      --> Identifier must refer to a procedure
//      --> In Oberon0, the selector cannot exist
//      --> Number of actual parameters must equal to the number of formal parameters
//      --> The types of the actual parameters must equal the types of the formal parameters
//      --> Constants, Literals or Expressions cannot be parsed as "VAR" parameters
void SemanticChecker::visit(ProcedureCallNode &node)
{

    // Check Name
    auto ident = node.get_ident();
    auto ident_info = scope_table_.lookup(ident->get_value());

    if (!ident_info)
    {
        logger_.error(ident->pos(), "Call of unknown procedure: '" + ident->get_value() + "'.");
        return;
    }

    if (ident_info->kind != Kind::PROCEDURE)
    {
        logger_.error(ident->pos(), "'" + ident->get_value() + "' does not refer to a procedure.");
        return;
    }

    // Check Selector
    auto selector = node.get_selector();
    if (selector && !selector->get_selector()->empty())
    {
        logger_.error(selector->pos(), "Call to array-index or record-field cannot refer to a procedure (In Oberon0).");
        return;
    }

    // Save Name in AST
    node.set_name(ident->get_value());

    // Get Function declaration
    auto *procedure_decl = dynamic_cast<ProcedureDeclarationNode *>(ident_info->node);
    int formal_parameter_nr = procedure_decl->get_parameter_number();
    auto actual_parameters = node.get_parameters();
    node.set_declaration(procedure_decl);

    if (!actual_parameters)
    {
        if (formal_parameter_nr == 0)
        {
            return;
        }
        else
        {
            logger_.error(node.pos(), "No parameters given for call to procedure '" + ident->get_value() + "'");
            return;
        }
    }

    if (static_cast<int>(actual_parameters->size()) != formal_parameter_nr)
    {
        logger_.error(node.pos(), "Number of actual parameters does not match the definition of '" + ident->get_value() + "' (Expected: " + to_string(formal_parameter_nr) + ", got: " + to_string(actual_parameters->size()) + ").");
        return;
    }

    // Type Checking Actual parameters
    auto formal_parameters = procedure_decl->get_parameters();
    auto act_param_itr = actual_parameters->begin();

    if (formal_parameters)
    {
        for (auto fp_section_itr = formal_parameters->begin(); fp_section_itr != formal_parameters->end(); fp_section_itr++)
        {

            auto curr_type = get_type(*std::get<2>(**fp_section_itr).get());

            for (auto formal_param = std::get<1>(**fp_section_itr)->begin(); formal_param != std::get<1>(**fp_section_itr)->end(); formal_param++)
            {

                if (curr_type.general == RECORD && curr_type.name == "RECORD")
                {
                    logger_.error(node.pos(), "Cannot correctly check type of given parameter for procedure '" + ident->get_value() + "' since record types can be compared by name only.");
                }
                else
                {
                    TypeInfo act_param_type = checkType(**act_param_itr);
                    if (act_param_type != curr_type)
                    {
                        logger_.error(node.pos(), "Type of actual parameter does not match type of formal parameter (expected '" + curr_type.name + "', got '" + act_param_type.name + "').");
                    }
                }

                // Constants/Literals cannot be passed as "VAR"
                if (std::get<0>(**fp_section_itr))
                {
                    if (evaluate_expression(**act_param_itr, true) || (**act_param_itr).getNodeType() != NodeType::ident_selector_expression)
                    {
                        logger_.error(node.pos(), "Constant/Literal expression passed as 'VAR' in call to procedure '" + ident->get_value() + "'.");
                    }
                }

                act_param_itr++;
            }
        }
    }
}

// Left empty, but needed to implement NodeVisitor
void SemanticChecker::visit(IntNode &node) { (void)node; }
void SemanticChecker::visit(ExpressionNode &node) { (void)node; }
void SemanticChecker::visit(UnaryExpressionNode &node) { (void)node; }
void SemanticChecker::visit(BinaryExpressionNode &node) { (void)node; }
void SemanticChecker::visit(IdentSelectorExpressionNode &node) { (void)node; }
void SemanticChecker::visit(SelectorNode &node) { (void)node; }

void SemanticChecker::validate_program(ModuleNode &node)
{
    visit(node);
}