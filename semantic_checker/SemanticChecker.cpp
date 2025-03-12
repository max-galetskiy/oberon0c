//
// Created by M on 15.12.2024.
//

#include "SemanticChecker.h"
#include "util/panic.h"

SemanticChecker::SemanticChecker(Logger &logger) : logger_(logger)
{
    scope_table_ = ScopeTable();
}

// Expressions:
//      --> Use of invalid types must be reported
//      --> Expressions must be type-checked

// Type Checking
//      --> Arithmetic Operators must have INTEGER values and return INTEGER
//      --> Comparison Operators must have the same type and  return BOOLEAN
//      --> Boolean Operators must have BOOLEAN values and return BOOLEAN
// Type is returned as a string with special types denotes by an underscore (as Oberon0 does not allow underscores as the start of identifiers)
std::shared_ptr<TypeInfo> SemanticChecker::checkType(ExpressionNode &expr)
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

            if (trace_type(l_type)->tag == RECORD ||
                trace_type(r_type)->tag == RECORD ||
                trace_type(l_type)->tag == ARRAY ||
                trace_type(r_type)->tag == ARRAY)
            {
                logger_.error(expr.pos(), "Illegal use of comparison operators with array/record types.");
                return error_type;
            }
            if (l_type->tag != r_type->tag || l_type->name != r_type->name)
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

            if (trace_type(checkType(*lhs))->tag != INTEGER)
            {
                logger_.error(expr.pos(), "LHS of arithmetic expression is not of type INTEGER.");
                return error_type;
            }
            if (trace_type(checkType(*rhs))->tag != INTEGER)
            {
                logger_.error(expr.pos(), "RHS of arithmetic expression is not of type INTEGER.");
                return error_type;
            }

            expr.set_types(integer_type, integer_type);
            return integer_type;
        }

        // Boolean Operators
        else if (op == SourceOperator::AND || op == SourceOperator::OR)
        {

            if (checkType(*lhs)->tag != BOOLEAN)
            {
                logger_.error(expr.pos(), "LHS of Boolean expression is not of type BOOLEAN.");
                return error_type;
            }
            if (checkType(*rhs)->tag != BOOLEAN)
            {
                logger_.error(expr.pos(), "RHS of Boolean expression is not of type BOOLEAN.");
                return error_type;
            }

            expr.set_types(boolean_type, boolean_type);
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
            if (checkType(*inner)->tag != BOOLEAN)
            {
                logger_.error(expr.pos(), "Cannot negate an expression that's not of type BOOLEAN.");
                return error_type;
            }

            expr.set_types(boolean_type, boolean_type);
            return boolean_type;
        }
        else if (op == SourceOperator::NEG)
        {
            if (trace_type(checkType(*inner))->tag != INTEGER)
            {
                logger_.error(expr.pos(), "Expression is not of type INTEGER.");
                return error_type;
            }
            expr.set_types(boolean_type, boolean_type);
            return integer_type;
        }
        else if (op == SourceOperator::NO_OPERATOR || op == SourceOperator::PAREN)
        {
            auto inner_type = checkType(*inner);
            expr.set_types(inner_type, trace_type(inner_type));
            return inner_type;
        }
    }
    else if (type == NodeType::ident_selector_expression)
    {

        auto id_expr = &dynamic_cast<IdentSelectorExpressionNode &>(expr);
        auto id_selector_type = check_selector_type(*id_expr);
        expr.set_types(id_selector_type, trace_type(id_selector_type));
        return id_selector_type;
    }
    else if (type == NodeType::integer)
    {
        expr.set_types(integer_type, integer_type);
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
std::shared_ptr<TypeInfo> SemanticChecker::check_selector_type(IdentSelectorExpressionNode &id_expr)
{
    auto identifier = id_expr.get_identifier();
    auto selector = id_expr.get_selector();

    // check if identifier is defined
    auto identifier_info = scope_table_.lookup(identifier->get_value());
    id_expr.get_identifier()->set_types(identifier_info->type, trace_type(identifier_info->type));
    if (!identifier_info)
    {
        logger_.error(id_expr.pos(), "Unknown Identifier: " + identifier->get_value());
        return error_type;
    }

    if (!selector || !selector->get_selector())
    {
        id_expr.set_types(identifier_info->type, trace_type(identifier_info->type));
        return identifier_info->type;
    }

    // Check the selector chain for validity (visit the selector)
    auto type = check_selector_chain(*identifier, *selector);
    id_expr.set_types(type, trace_type(type));
    return type;
}

// Traces back types to handle type-aliases like the following:
// TYPE INTARRAY = ARRAY 20 OF INTEGER
// TYPE INA      = INTARRAY
// TYPE ABC      = INA
std::shared_ptr<TypeInfo> SemanticChecker::trace_type(std::shared_ptr<TypeInfo> initial_type)
{

    if (!initial_type || initial_type->tag != ALIAS)
    {
        return initial_type;
    }

    auto current_type = initial_type;
    IdentInfo *curr_info;

    while ((curr_info = scope_table_.lookup(initial_type->name)) && (current_type->tag == ALIAS))
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
std::shared_ptr<TypeInfo> SemanticChecker::check_selector_chain(IdentNode &ident, SelectorNode &selector)
{
    IdentInfo *prev_info = scope_table_.lookup(ident.get_value());
    ident.set_types(prev_info->type, trace_type(prev_info->type));

    if (!selector.get_selector())
    {
        return prev_info->type;
    }

    if(!prev_info){
        logger_.error(ident.pos(),"Unknown identifier: " + ident.get_value());
        return error_type;
    }

    auto chain = selector.get_selector();
    auto prev_type = prev_info->type;

    // Traverse entire chain
    // Note that the validity of the first selector cannot be checked inside this function
    for (auto itr = chain->begin(); itr != chain->end(); itr++)
    {

        if (std::get<0>(*itr))
        {

            // Array Index

            // Object must be an array type
            if (trace_type(prev_type)->tag != ARRAY)
            {
                logger_.error(selector.pos(), "Tried to index non-array object.");
                return error_type;
            }

            // Index must evaluate to a non-negative integer
            auto expr = std::get<2>(*itr);
            if (trace_type(checkType(*expr))->tag != INTEGER)
            {
                logger_.error(selector.pos(), "Array index does not evaluate to an integer.");
                return error_type;
            }

            auto arr_type = trace_type(prev_type);
            int dim = std::get<ArrayTypeInfo>(arr_type->extended_info.value()).size;

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
            TypeInfo* elem_type = std::get<ArrayTypeInfo>(arr_type->extended_info.value()).elementType.get();

            if (elem_type->tag == INTEGER)
            {
                prev_type = scope_table_.lookup_type(int_string);
            }
            else if (elem_type->tag == ALIAS)
            {
                prev_type = scope_table_.lookup_type(elem_type->name);
            }

            if (!prev_type)
            {
                logger_.error(selector.pos(), "Unable to get information for type '" + elem_type->name + "'");
                return error_type;
            }
        }
        else
        {

            // Record Field

            // Object must actually have a record type
            if (trace_type(prev_type)->tag != RECORD)
            {
                logger_.error(selector.pos(), "Tried to access field of a non-record object.");
                return error_type;
            }

            // Identifier must refer to an actual field of that record type
            auto field_type = scope_table_.lookup_field(prev_type->name, std::get<1>(*itr)->get_value());
            if (!field_type || field_type->tag == ERROR_TAG)
            {
                logger_.error(selector.pos(), "Tried to access invalid field of record type '" + prev_type->name + "' (Field: " + std::get<1>(*itr)->get_value() + ").");
                return error_type;
            }

            // Update prev_type
            prev_type = field_type;
        }
    }

    return prev_type;
}

// Verifies, creates and inserts new type into typetable
std::shared_ptr<TypeInfo> SemanticChecker::create_new_type(TypeNode &type, const string &type_name, bool insert_into_table) {

    // Identifier:
    //      --> Must refer to a valid type
    if(type.getNodeType() == NodeType::ident){
        auto ident_node = dynamic_cast<IdentNode &>(type);
        const string ident_name = ident_node.get_value();

        if(!scope_table_.lookup_type(ident_name)){
            if(!scope_table_.lookup(ident_name)){
                logger_.error(type.pos(), "Use of unknown identifier: '" + ident_name + "'.");
                return error_type;
            }else{
                logger_.error(type.pos(), "Identifier '" + ident_name + "' does not refer to a type.");
                return error_type;
            }
        }

        return (insert_into_table) ? scope_table_.insert_type(type_name,ident_name) : scope_table_.lookup_type(ident_name);
    }

    // ArrayType:
    //      --> Specified dimension must evaluate to an integer greater than zero
    //      --> Specified type must exist (and be a type)
    else if(type.getNodeType() == NodeType::array_type){
        auto array_node = &dynamic_cast<ArrayTypeNode &>(type);
        auto dim = evaluate_expression(*array_node->get_dim_node());

        if(!dim.has_value()){
            logger_.error(array_node->pos(), "Specified array dimensions do not evaluate to a constant.");
            return error_type;
        }
        else if (dim.value() <= 0)
        {
            logger_.error(array_node->pos(), "Cannot create array of size " + to_string(dim.value()) + ".");
        }
        else
        {
            array_node->set_dim(dim.value());
        }

        auto elem_typenode = array_node->get_type_node();
        auto elem_type = create_new_type(*elem_typenode,"",false);

        return (insert_into_table)? scope_table_.insert_type(type_name, elem_type,dim.value()) : std::make_shared<TypeInfo>(type_name,ARRAY,ArrayTypeInfo(elem_type,dim.value()));
    }

    // RecordType:
    //      --> RecordType definition opens a new scope
    //      --> All field names must be unique
    //      --> Types of the fields must be valid
    else if(type.getNodeType() == NodeType::record_type){
        auto record_node = &dynamic_cast<RecordTypeNode&>(type);
        auto field_map = key_value_map(*record_node);               // Note: This function also handles some recordType errors

        // Insert record type into table
        auto record_type = (insert_into_table)? scope_table_.insert_type(type_name,field_map) : std::make_shared<TypeInfo>(type_name,RECORD,RecordTypeInfo(field_map));

        // Insert Traced Record Types into RecordTypeNode
        auto field_map_opt = scope_table_.lookup_record(type_name);
        if (field_map_opt)
        {
            auto fields = field_map_opt.value();

            // Trace Types
            for (auto fitr = field_map.begin(); fitr != field_map.end(); fitr++)
            {
                fitr->second = trace_type(fitr->second);
            }

            record_node->insert_field_types(fields);
        }

        return record_type;
    }

    panic("Invalid NodeType passed as TypeNode!");
}


// Module:
//      --> The beginning and ending names should align
//      --> A module opens a new scope
void SemanticChecker::visit(ModuleNode &module)
{

    scope_table_.beginScope();

    // Insert pre-defined types "INTEGER" and "BOOLEAN"
    scope_table_.insert_type(int_string,INTEGER);
    scope_table_.insert_type(bool_string,BOOLEAN);

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
    scope_table_.insert(names.first->get_value(), Kind::PROCEDURE, &procedure, error_type);

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
            std::shared_ptr<TypeInfo> var_type = nullptr;
            if (type->getNodeType() == NodeType::array_type || type->getNodeType() == NodeType::record_type)
            {
                logger_.error(type->pos(), "New Type defined in formal parameters of function. Actual Parameter will never be able to fulfill this type (Note: The Oberon0 compiler follows name-equivalence, not structural equivalence).");
            }
            else
            {

                // Corresponding Type has to be looked up
                auto type_info = scope_table_.lookup_type(dynamic_cast<IdentNode *>(type)->get_value());
                var_type = (type_info) ? type_info : error_type;
            }
            visit(*type);

            for (auto var = std::get<1>(**itr)->begin(); var != std::get<1>(**itr)->end(); var++)
            {

                // Check for double definitions
                if (scope_table_.lookup(var->get()->get_value(), true))
                {
                    logger_.error(var->get()->pos(), "Multiple use of the same parameter name.");
                }

                var->get()->set_types(var_type, trace_type(var_type));
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
        itr->first->set_types(const_type, trace_type(const_type));
        scope_table_.insert(itr->first->get_value(), Kind::CONSTANT, itr->second, const_type);
    }

    // Typenames
    //      --> Type definition must be valid
    for (auto itr = typenames.begin(); itr != typenames.end(); itr++)
    {

        // check for double declarations
        if (scope_table_.lookup(itr->first->get_value(), true))
        {
            if(itr->first->get_value() == int_string || itr->first->get_value() == bool_string){
                logger_.error(declars.pos(), "Attempt to redefine predefined type '" + itr->first->get_value() + "'.");
            }else{
                logger_.error(declars.pos(), "Multiple Declarations of identifier '" + itr->first->get_value() + "'.");
            }

        }

        // check the type definition and insert into scope table
        auto type = create_new_type(*itr->second, itr->first->get_value(),true);

        itr->first->set_types(type, trace_type(type));
        itr->second->set_types(type, trace_type(type));
    }

    // Variables:
    //      --> Type must be valid
    for (auto itr = variables.begin(); itr != variables.end(); itr++)
    {

        // check for valid types (i.e., visit Type Node)
        visit(*itr->second);

        std::shared_ptr<TypeInfo> var_type;

        // Assign type (if right hand side is an identifier, then the corresponding type info has to be looked up
        // Correctly Assign TypeNode
        if (itr->second->getNodeType() == NodeType::ident)
        {
            var_type = scope_table_.lookup_type(dynamic_cast<IdentNode *>(itr->second)->get_value());
        }
        else
        {
            var_type = create_new_type(*itr->second,"", false);
        }

        for (auto el = itr->first.begin(); el != itr->first.end(); el++)
        {

            // check for double declarations (only in current scope)
            if (scope_table_.lookup((*el)->get_value(), true))
            {
                logger_.error(declars.pos(), "Multiple Declarations of identifier '" + (*el)->get_value() + "'.");
            }

            // insert variable into symbol table
            (*el)->set_types(var_type, trace_type(var_type));
            scope_table_.insert((*el)->get_value(), Kind::VARIABLE, itr->second, var_type);
        }
    }

    // Procedures
    for (auto itr = procedures.begin(); itr != procedures.end(); itr++)
    {
        visit(*(*itr));
    }
}

// Fills a key-value-map-vector which is needed to place record types into the scope table
std::map<string,std::shared_ptr<TypeInfo>> SemanticChecker::key_value_map(RecordTypeNode &node)
{
    scope_table_.beginScope();
    std::map<string,std::shared_ptr<TypeInfo>> key_value_map;

    auto fields = node.get_fields();
    for (auto field_itr = fields.begin(); field_itr != fields.end(); field_itr++)
    {

        // Check for valid type definition
        auto field_type = create_new_type(*field_itr->second,"", false);

        for (auto field_name = field_itr->first.begin(); field_name != field_itr->first.end(); field_name++)
        {
            // Check for double names
            if (scope_table_.lookup(*field_name, true))
            {
                logger_.error(node.pos(), "Multiple definitions of record field '" + *field_name + "'.");
            }

            // Note: Since this scope is only used to check for double definitions and immediately closed after that,
            //       It doesn't *really* matter what we put into the scope table for the fields
            scope_table_.insert(*field_name, Kind::VARIABLE, nullptr, error_type);

            key_value_map[*field_name] = field_type;
        }
    }

    scope_table_.endScope();
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
        logger_.error(node.pos(), "Use of unknown identifier: '" + lhs_id + "'.");
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
        if (lhs_type->tag == ERROR_TAG)
        {
            return;
        }
    }

    // Check RHS
    auto rhs = node.get_expr();
    auto expr_type = checkType(*rhs);

    if ((*expr_type != *lhs_type) && expr_type->tag != ERROR_TAG)
    { // We exclude the error case to avoid too many exceptions
        logger_.error(node.pos(), "Cannot assign something of type '" + expr_type->name + "' to a variable of type '" + lhs_type->name + "'.");
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
    if (checkType(*condition)->tag != BOOLEAN)
    {
        logger_.error(condition->pos(), "Condition of If-Statement does not evaluate to a BOOLEAN.");
    }
    visit(*node.get_then());

    // Else-Ifs
    auto else_ifs = node.get_else_ifs();
    for (auto itr = else_ifs->begin(); itr != else_ifs->end(); itr++)
    {

        if (checkType(*itr->first)->tag != BOOLEAN)
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
    if (checkType(*condition)->tag != BOOLEAN)
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
    if (checkType(*condition)->tag != BOOLEAN)
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

            auto curr_type = create_new_type(*std::get<2>(**fp_section_itr).get(),"",false);

            for (auto formal_param = std::get<1>(**fp_section_itr)->begin(); formal_param != std::get<1>(**fp_section_itr)->end(); formal_param++)
            {

                if (curr_type->tag == RECORD && curr_type->name == "RECORD")
                {
                    logger_.error(node.pos(), "Cannot correctly check type of given parameter for procedure '" + ident->get_value() + "' since record types can be compared by name only.");
                }
                else
                {
                    auto act_param_type = checkType(**act_param_itr);
                    if (*act_param_type != *curr_type)
                    {
                        logger_.error(node.pos(), "Type of actual parameter does not match type of formal parameter (expected '" + curr_type->name + "', got '" + act_param_type->name + "').");
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
void SemanticChecker::visit(TypeNode &node){(void)node;}
void SemanticChecker::visit(IdentNode &node){(void)node;}
void SemanticChecker::visit(ArrayTypeNode &node){(void)node;}
void SemanticChecker::visit(RecordTypeNode &node){(void)node;}

void SemanticChecker::validate_program(ModuleNode &node)
{
    visit(node);
}


