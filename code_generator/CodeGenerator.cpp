//
// Created by M on 19.01.2025.
//

#include "CodeGenerator.h"
#include "../util/panic.h"
#include <utility>

void CodeGenerator::init_target_machine()
{
    // initialize LLVM
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();
    // use default target triple of host
    std::string triple = sys::getDefaultTargetTriple();
    // set up target
    std::string error;
    auto target = TargetRegistry::lookupTarget(triple, error);
    if (!target)
    {
        std::cerr << error << std::endl;
    }
    else
    {
        // set up target machine to match host
        std::string cpu = "generic";
        std::string features;
        TargetOptions opt;

#ifdef _LLVM_LEGACY
        auto model = llvm::Optional<Reloc::Model>();
#else
        auto model = std::optional<Reloc::Model>();
#endif

        target_ = target->createTargetMachine(triple, cpu, features, opt, model);
    }
}

void CodeGenerator::init_builder()
{
    init_target_machine();
    builder_ = new IRBuilder<>(ctx_);

    if (!target_)
    {
        return;
    }

    // set up LLVM module
    module_ = new Module(filename_, ctx_);
    module_->setDataLayout(target_->createDataLayout());
    module_->setTargetTriple(target_->getTargetTriple().getTriple());
}

CodeGenerator::CodeGenerator(string filename, OutputFileType output_type) : output_type_(output_type), filename_(std::move(filename))
{
    init_target_machine();
    init_builder();
}

void CodeGenerator::visit(ExpressionNode &node)
{
    switch (node.getNodeType())
    {
    case NodeType::unary_expression:
        visit(dynamic_cast<UnaryExpressionNode &>(node));
        break;
    case NodeType::binary_expression:
        visit(dynamic_cast<BinaryExpressionNode &>(node));
        break;
    case NodeType::ident_selector_expression:
        visit(dynamic_cast<IdentSelectorExpressionNode &>(node));
        break;
    case NodeType::integer:
        visit(dynamic_cast<IntNode &>(node));
        break;
    default:
        panic("unreachable");
    }
}

void CodeGenerator::visit(BinaryExpressionNode &expr)
{

    auto op = expr.get_op();

    expr.get_lhs()->accept(*this);
    llvm::Value *lhsValue = value_;

    // add Short-Circuit Evaluation

    expr.get_rhs()->accept(*this);
    llvm::Value *rhsValue = value_;

    switch (op)
    {

    // math operation
    case SourceOperator::PLUS:
        value_ = builder_->CreateAdd(lhsValue, rhsValue, "add");
        break;
    case SourceOperator::MINUS:
        value_ = builder_->CreateSub(lhsValue, rhsValue, "sub");
        break;
    case SourceOperator::MULT:
        value_ = builder_->CreateMul(lhsValue, rhsValue, "mul");
        break;
    case SourceOperator::DIV:
        value_ = builder_->CreateSDiv(lhsValue, rhsValue, "div");
        break;

    case SourceOperator::MOD:
        value_ = builder_->CreateSRem(lhsValue, rhsValue, "mod");
        break;
    // logical operation
    case SourceOperator::OR:
        value_ = builder_->CreateOr(lhsValue, rhsValue, "or");
        break;

    case SourceOperator::AND:
        value_ = builder_->CreateAnd(lhsValue, rhsValue, "and");
        break;
    // comp operation
    case SourceOperator::EQ:
        value_ = builder_->CreateICmpEQ(lhsValue, rhsValue, "eq");
        break;
    case SourceOperator::NEQ:
        value_ = builder_->CreateICmpNE(lhsValue, rhsValue, "neq");
        break;
    case SourceOperator::LT:
        value_ = builder_->CreateICmpSLT(lhsValue, rhsValue, "lt");
        break;
    case SourceOperator::LEQ:
        value_ = builder_->CreateICmpSLE(lhsValue, rhsValue, "leq");
        break;
    case SourceOperator::GT:
        value_ = builder_->CreateICmpSGT(lhsValue, rhsValue, "gt");
        break;
    case SourceOperator::GEQ:
        value_ = builder_->CreateICmpSGE(lhsValue, rhsValue, "geq");
        break;

    default:
        panic("Unsupported binary operator");
    }
}

void CodeGenerator::visit(UnaryExpressionNode &expr)

{
    expr.get_expr()->accept(*this);
    llvm::Value *exprValue = value_;

    SourceOperator op = expr.get_op();

    switch (op)
    {
    case SourceOperator::NEG:
        value_ = builder_->CreateNeg(exprValue, "neg");
        break;

    case SourceOperator::NOT:
        value_ = builder_->CreateNot(exprValue, "not");
        break;

    case SourceOperator::NO_OPERATOR:
    case SourceOperator::PAREN:
        value_ = exprValue;
        break;

    default:
        panic("Unsupported unary operator");
    }
}

void CodeGenerator::visit(IdentSelectorExpressionNode &node)
{

    auto ident = node.get_identifier();
    assert(!ident->get_type_embedding().has_value());
    LoadIdentSelector(*ident, node.get_selector());
}

void CodeGenerator::LoadIdentSelector(IdentNode &ident, SelectorNode *selector, bool return_pointer)
{
    std::string name = ident.get_value();
    auto p = variables_.lookup(name);

    if (!p)
    {
        panic("Failed lookup for identifier '" + ident.get_value() + "'.");
    }

    llvm::Value *var = std::get<0>(*p);
    TypeInfoClass *type = (std::get<1>(*p)).get();

    if (!selector || selector->get_selector()->empty())
    {

        LoadIdent(ident, return_pointer);
        return;
    }

    auto selectors = *(selector->get_selector());

    assert(!selectors.empty());

    llvm::Value *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), 0);

    for (size_t i = 0; i < selectors.size(); i++)
    {
        auto &tuple_ref = selectors[i];
        auto &[is_array, ident_ptr, expr_ptr] = tuple_ref;

        if (!expr_ptr) // Record field access
        {
            assert(type->tag == TypeTag::RECORD_TAG);
            auto &record_fields = std::get<TypeInfoClass::Record>(type->value).fields;

            assert(ident_ptr);
            std::string field_name = ident_ptr->get_value();

            // Find the field index
            int field_index = -1;
            for (size_t j = 0; j < record_fields.size(); j++)
            {
                if (record_fields[j].first == field_name)
                {
                    field_index = j;
                    break;
                }
            }
            assert(field_index < 0 && "Field not found in record");
            auto idx = static_cast<uint64_t>(field_index);

            llvm::Value *field_index_val = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), idx);
            var = builder_->CreateGEP(type->llvmType[0], var, {zero, field_index_val}, "rec_field_" + field_name);

            type = record_fields[idx].second.get();
        }
        else // Array access
        {
            assert(type->tag == TypeTag::ARRAY_TAG);
            assert(!ident_ptr);

            expr_ptr->accept(*this);
            llvm::Value *index_val = value_;

            assert(type->llvmType.size() == 1);
            var = builder_->CreateGEP(type->llvmType[0], var, {zero, index_val}, "arr_ptr_" + name);

            type = std::get<TypeInfoClass::Array>(type->value).elementType.get();
        }
    }

    assert(type->llvmType.size() == 1);

    if (return_pointer)
    {
        value_ = var;
    }
    else
    {
        llvm::Value *val = builder_->CreateLoad(type->llvmType[0], var, "load_" + name);
        value_ = val;
    }
}

void CodeGenerator::visit(IdentNode &ident)
{
    if (ident.get_type_embedding().has_value())
    {
        auto a = ident.get_type_embedding().value();
        assert(a.general == GeneralType::BOOLEAN || a.general == GeneralType::INTEGER);

        if (a.general == GeneralType::BOOLEAN)
        {
            temp_type_ = *type_table_.lookup("BOOLEAN");
        }
        else
        {
            temp_type_ = *type_table_.lookup("INTEGER");
        }

        return;
    }
    LoadIdent(ident, false);
}

void CodeGenerator::LoadIdent(IdentNode &ident, bool return_pointer)
{
    assert(!ident.get_type_embedding().has_value());
    std::string name = ident.get_value();
    auto p = variables_.lookup(name);

    if (!p)
    {
        panic("Failed lookup for identifier '" + ident.get_value() + "'.");
    }

    llvm::Value *var = std::get<0>(*p);
    TypeInfoClass *type = std::get<1>(*p).get();
    bool is_pointer = std::get<2>(*p);

    // llvm::Value *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), 0);

    assert(type->llvmType.size() == 1);
    if (!var->getType()->isPointerTy() || return_pointer)
    {
        value_ = var;
    }
    else
    {

        if (is_pointer)
        {
            auto val = builder_->CreateLoad(type->llvmType[0]->getPointerTo(), var, "load_" + name);
            auto deref = builder_->CreateLoad(type->llvmType[0], val, "deref_" + name);
            value_ = deref;
        }
        else
        {
            llvm::Value *val = builder_->CreateLoad(type->llvmType[0], var, "load_" + name);
            value_ = val;
        }
    }
}

void CodeGenerator::visit(IntNode &val)
{
    long int_val = val.get_value();
    llvm::Type *longType = llvm::Type::getInt64Ty(ctx_);
    auto *longValue = llvm::ConstantInt::get(longType, static_cast<long unsigned int>(int_val));
    value_ = longValue;
}

void CodeGenerator::visit(SelectorNode &)
{
    panic("unreachable this should be handled in IdentSelectorExpressionNode");
}

void CodeGenerator::visit(DeclarationsNode &node)
{
    create_declarations(node, false);
}

void CodeGenerator::create_declarations(DeclarationsNode &node, bool is_global)
{

    auto types = node.get_typenames();
    for (auto it = types.begin(); it != types.end(); ++it)
    {
        auto name = it->first->get_value();
        it->second->accept(*this);
        auto type = temp_type_;

        // resolve/create llvm type
        switch (type.tag)
        {
        case TypeTag::RECORD_TAG:
            assert(type.llvmType.size() >= 1);
            type.llvmType = {llvm::StructType::create(ctx_, type.llvmType, name)};
            break;
        case TypeTag::ARRAY_TAG:
            assert(type.llvmType.size() == 1);
            assert(std::get<1>(type.value).elementType);
            type.llvmType = {llvm::ArrayType::get(std::get<1>(type.value).elementType->llvmType[0], static_cast<long unsigned int>(std::get<1>(type.value).size))};
            break;
        case TypeTag::INTEGER_TAG:
            assert(type.llvmType.size() == 1);
            type.llvmType = {llvm::Type::getInt64Ty(ctx_)};
            break;
        case TypeTag::BOOLEAN_TAG:
            assert(type.llvmType.size() == 1);
            type.llvmType = {llvm::Type::getInt1Ty(ctx_)};
            break;
        default:
            panic("unreachable (faulty tag)");
        }

        type_table_.insert(name, type);
    }

    auto constants = node.get_constants();
    for (auto it = begin(constants); it != end(constants); ++it)
    {

        auto ident = it->first;

        assert(!ident->get_type_embedding().has_value());
        std::string name = ident->get_value();

        auto type = type_table_.lookup(ident->get_actual_type().name);
        assert(type->llvmType.size() == 1);

        it->second->accept(*this);
        llvm::Value *value = value_;

        llvm::Value *var;
        if (is_global)
        {
            var = new GlobalVariable(*module_, type->llvmType[0], false, GlobalValue::InternalLinkage, dyn_cast<Constant>(value), name);
        }
        else
        {
            var = builder_->CreateAlloca(type->llvmType[0], nullptr, name);
            builder_->CreateStore(value, var);
        }

        variables_.insert(ident->get_value(), var, std::make_shared<TypeInfoClass>(*type));
    }

    auto variables = node.get_variables();
    for (auto it = begin(variables); it != end(variables); ++it)
    {
        for (auto ident_itr = it->first.begin(); ident_itr != it->first.end(); ident_itr++)
        {
            auto ident = *ident_itr;

            assert(!ident->get_type_embedding().has_value());
            std::string name = ident->get_value();

            auto type = type_table_.lookup(ident->get_actual_type().name);
            assert(type->llvmType.size() == 1);

            llvm::Value *var;
            if (is_global)
            {
                var = new GlobalVariable(*module_, type->llvmType[0], false, GlobalValue::InternalLinkage, Constant::getNullValue(type->llvmType[0]), name);
            }
            else
            {
                var = builder_->CreateAlloca(type->llvmType[0], nullptr, name);
            }

            variables_.insert(ident->get_value(), var, std::make_shared<TypeInfoClass>(*type));
        }
    }

    auto procedures = node.get_procedures();
    for (auto itr = procedures.begin(); itr != procedures.end(); itr++)
    {
        visit(**itr);
    }
}

void CodeGenerator::visit(TypeNode &node)
{
    switch (node.getNodeType())
    {
    case NodeType::ident:
        visit(dynamic_cast<IdentNode &>(node));
        break;
    case NodeType::array_type:
        visit(dynamic_cast<ArrayTypeNode &>(node));
        break;
    case NodeType::record_type:
        visit(dynamic_cast<RecordTypeNode &>(node));
        break;
    default:
        panic("unreachable");
    }
}

void CodeGenerator::visit(ArrayTypeNode &node)
{
    auto dim = node.get_dim();
    assert(dim.has_value());
    int val = dim.value();
    assert(val > 0);

    string type_name = node.get_base_type_info().name;
    auto info_class = type_table_.lookup(type_name);

    temp_type_.tag = TypeTag::ARRAY_TAG;
    temp_type_.llvmType = {info_class->llvmType[0]->getPointerTo()};
    temp_type_.value.emplace<TypeInfoClass::Array>(std::make_shared<TypeInfoClass>(*info_class), val);
}

void CodeGenerator::visit(RecordTypeNode &node)
{
    auto raw_fields = node.get_fields();
    auto field_types = *node.get_field_types();

    std::vector<std::pair<std::string, std::shared_ptr<TypeInfoClass>>> info_fields;
    std::vector<llvm::Type *> llvm_fields;

    for (auto it = begin(raw_fields); it != end(raw_fields); ++it)
    {
        auto names = it->first;
        for (auto field_it = begin(names); field_it != end(names); ++field_it)
        {

            std::string name = *field_it;

            assert(field_types.find(name) != field_types.end());

            TypeInfo type_info = field_types[name];

            TypeInfoClass *info_class = type_table_.lookup(type_info.name);
            assert(info_class->llvmType.size() == 1);
            llvm_fields.push_back(info_class->llvmType[0]);
            info_fields.emplace_back(name, std::make_shared<TypeInfoClass>(*info_class));
        }
    }

    temp_type_.llvmType = llvm_fields;
    temp_type_.tag = TypeTag::RECORD_TAG;
    get<TypeInfoClass::Record>(temp_type_.value).fields = info_fields;
}

void CodeGenerator::visit(ProcedureDeclarationNode &node)
{

    variables_.beginScope();
    type_table_.beginScope();

    auto prev_block = builder_->GetInsertBlock();

    auto name = node.get_names().first->get_value();
    auto arguments = node.get_parameters();

    // Create Signature
    std::vector<Type *> llvm_params;

    if (arguments)
    {
        for (auto itr = arguments->begin(); itr != arguments->end(); itr++)
        {

            bool is_var = std::get<0>(**itr);
            auto idents = std::get<1>(**itr).get();
            auto typenode = std::get<2>(**itr).get();

            // Determine LLVM Type
            if (typenode->getNodeType() == NodeType::array_type || typenode->getNodeType() == NodeType::record_type)
            {
                panic("New type defined in declaration of procedure '" + name + "'.");
            }

            auto type_info = type_table_.lookup(dynamic_cast<IdentNode *>(typenode)->get_value());
            if (!type_info)
            {
                panic("No type information found for '" + dynamic_cast<IdentNode *>(typenode)->get_value() + "'.");
            }

            llvm::Type *llvm_type = type_info->llvmType.at(0);
            if (is_var)
            {
                llvm_type = llvm_type->getPointerTo();
            }

            for (auto param = idents->begin(); param != idents->end(); param++)
            {
                variables_.insert(param->get()->get_value(), nullptr, std::make_shared<TypeInfoClass>(*type_info), is_var);
                llvm_params.push_back(llvm_type);
            }
        }
    }

    auto signature = FunctionType::get(builder_->getVoidTy(), llvm_params, false);

    // Define Function and add it to FunctionList
    auto procedure = module_->getOrInsertFunction(name, signature);
    auto function = cast<Function>(procedure.getCallee());
    procedures_[name] = function;

    // Define BasicBlock
    auto block = BasicBlock::Create(builder_->getContext(), "entry", function);
    builder_->SetInsertPoint(block);

    // Set the argument names and update the variables_ table entries
    if (arguments)
    {
        auto arg_itr = function->arg_begin();
        for (auto itr = arguments->begin(); itr != arguments->end(); itr++)
        {
            bool is_var = std::get<0>(**itr);
            auto idents = std::get<1>(**itr).get();
            for (auto param = idents->begin(); param != idents->end(); param++)
            {

                if (arg_itr == function->arg_end())
                {
                    panic("Number of arguments in source-code function does not equal number of arguments of LLVM-Function-Type");
                }

                auto arg_info = variables_.lookup(param->get()->get_value());
                if (!arg_info)
                {
                    panic("Failed lookup for identifier '" + param->get()->get_value() + "'.");
                }

                // Reserve place for argument on the stack
                auto param_type = (is_var)? std::get<1>(*arg_info)->llvmType[0]->getPointerTo() : std::get<1>(*arg_info)->llvmType[0];
                auto param_value = builder_->CreateAlloca(param_type, nullptr,param->get()->get_value());
                builder_->CreateStore(arg_itr,param_value);
                std::get<0>(*arg_info) = param_value;

                arg_itr->setName(param->get()->get_value());
                arg_itr++;
            }
        }
    }


    // Add Procedure Declaration and Statements
    visit(*node.get_declarations());
    visit(*node.get_statements());

    variables_.endScope();
    type_table_.endScope();

    // Add Return
    builder_->CreateRetVoid();
    builder_->SetInsertPoint(prev_block);
}

void CodeGenerator::visit(StatementNode &node)
{
    switch (node.getNodeType())
    {
    case NodeType::assignment:
        visit(dynamic_cast<AssignmentNode &>(node));
        break;
    case NodeType::if_statement:
        visit(dynamic_cast<IfStatementNode &>(node));
        break;
    case NodeType::procedure_call:
        visit(dynamic_cast<ProcedureCallNode &>(node));
        break;
    case NodeType::repeat_statement:
        visit(dynamic_cast<RepeatStatementNode &>(node));
        break;
    case NodeType::while_statement:
        visit(dynamic_cast<WhileStatementNode &>(node));
        break;
    default:
        return;
    }
}

void CodeGenerator::visit(AssignmentNode &node)
{

    auto ident = node.get_variable();
    auto selector = node.get_selector();
    auto expr = node.get_expr();

    visit(*expr);
    auto value = value_;

    LoadIdentSelector(*ident, selector, true);
    builder_->CreateStore(value, value_);
}

void CodeGenerator::visit(IfStatementNode &node)
{

    // Set up initial Blocks
    auto then = BasicBlock::Create(builder_->getContext(), "then", builder_->GetInsertBlock()->getParent());
    // Set up Else-If blocks
    std::vector<BasicBlock *> cond_branches;
    std::vector<BasicBlock *> then_branches;
    auto else_ifs = node.get_else_ifs();
    int branch_counter = 0;
    if (else_ifs)
    {
        for (auto itr = else_ifs->begin(); itr != else_ifs->end(); itr++)
        {
            string cond_branch_name = to_string(branch_counter) + "_cond";
            string then_branch_name = to_string(branch_counter) + "_then";
            cond_branches.push_back(BasicBlock::Create(builder_->getContext(), cond_branch_name, builder_->GetInsertBlock()->getParent()));
            then_branches.push_back(BasicBlock::Create(builder_->getContext(), then_branch_name, builder_->GetInsertBlock()->getParent()));
            branch_counter++;
        }
    }

    // Set up Else block
    auto else_block = (!node.get_else()) ? nullptr : BasicBlock::Create(builder_->getContext(), "else", builder_->GetInsertBlock()->getParent());

    // Set up post-branch block
    auto post_branch = BasicBlock::Create(builder_->getContext(), "post_branch", builder_->GetInsertBlock()->getParent());

    // Evaluate initial condition
    visit(*node.get_condition());
    auto initial_cond = value_;

    // Create initial branch
    if (cond_branches.empty() && node.get_else())
    {
        builder_->CreateCondBr(initial_cond, then, else_block);
    }
    else if (cond_branches.empty() && !node.get_else())
    {
        builder_->CreateCondBr(initial_cond, then, post_branch);
    }
    else
    {
        builder_->CreateCondBr(initial_cond, then, cond_branches[0]);

        // Populate Condition Branches
        for (long unsigned int i = 0; i < cond_branches.size(); i++)
        {

            builder_->SetInsertPoint(cond_branches[i]);
            visit(*(*else_ifs)[i].first);
            auto cond = value_;

            if (i < cond_branches.size() - 1)
            {
                builder_->CreateCondBr(cond, then_branches[i], cond_branches[i + 1]);
            }
            else
            {
                builder_->CreateCondBr(cond, then_branches[i], else_block);
            }
        }
    }

    // Populate initial then-block
    builder_->SetInsertPoint(then);
    visit(*node.get_then());
    builder_->CreateBr(post_branch);

    // Populate "Then" Blocks
    if (else_ifs)
    {
        for (long unsigned int i = 0; i < then_branches.size(); i++)
        {
            builder_->SetInsertPoint(then_branches[i]);
            visit(*(*else_ifs)[i].second);
            builder_->CreateBr(post_branch);
        }
    }

    // Populate "Else" Block
    if (node.get_else())
    {
        builder_->SetInsertPoint(else_block);
        visit(*node.get_else());
        builder_->CreateBr(post_branch);
    }

    // Continue post branch
    builder_->SetInsertPoint(post_branch);
}

void CodeGenerator::visit(ProcedureCallNode &node)
{
    // Get ProcedureName
    auto procedure_name = node.get_name();
    if (procedures_.find(procedure_name) == procedures_.end())
    {
        panic("Code generator could not find procedure '" + procedure_name + "' in procedure_list.");
    }

    // Get Arguments
    if (!node.get_declaration())
    {
        panic("Declaration node pointer not set for call to procedure '" + procedure_name + "'.");
    }

    auto formal_parameters = node.get_declaration()->get_parameters();
    auto actual_parameters = node.get_parameters();
    std::vector<Value *> arguments;

    if (formal_parameters && actual_parameters)
    {
        auto act_itr = actual_parameters->begin();
        for (auto param_outer = formal_parameters->begin(); param_outer != formal_parameters->end(); param_outer++)
        {

            bool is_var = std::get<0>(**param_outer);
            auto ident_list = std::get<1>(**param_outer).get();

            for (auto formal_param = ident_list->begin(); formal_param != ident_list->end(); formal_param++)
            {

                if (act_itr == actual_parameters->end())
                {
                    panic("Number of actual parameters does not equal number of formal parameters for call to procedure '" + procedure_name + "'.");
                }

                // Generate argument

                if (is_var)
                {
                    if ((**act_itr).getNodeType() != NodeType::ident_selector_expression)
                    {
                        panic("Constant expression for VAR argument in call to '" + procedure_name + "'.");
                    }

                    auto id_expr = dynamic_cast<IdentSelectorExpressionNode *>((*act_itr).get());
                    LoadIdentSelector(*(id_expr->get_identifier()), id_expr->get_selector(), true);
                }
                else
                {
                    visit(**act_itr);
                }

                arguments.push_back(value_);
                act_itr++;
            }
        }
    }

    // Create Call
    builder_->CreateCall(procedures_[procedure_name], arguments);
}

void CodeGenerator::visit(RepeatStatementNode &node)
{
    // Create Basic Blocks
    auto loop = BasicBlock::Create(builder_->getContext(), "loop", builder_->GetInsertBlock()->getParent());
    auto tail = BasicBlock::Create(builder_->getContext(), "tail", builder_->GetInsertBlock()->getParent());

    // Enter loop
    builder_->CreateBr(loop);
    builder_->SetInsertPoint(loop);

    // Loop statements
    visit(*node.get_statements());

    // Check condition
    visit(*node.get_expr());
    auto cond = value_;

    // Jump forwards if condition is true, otherwise backwards
    builder_->CreateCondBr(cond, tail, loop);

    builder_->SetInsertPoint(tail);
}

void CodeGenerator::visit(StatementSequenceNode &node)
{
    auto statements = node.get_statements();
    if (!statements)
    {
        return;
    }

    for (auto itr = statements->begin(); itr != statements->end(); itr++)
    {
        visit(**itr);
    }
}

void CodeGenerator::visit(WhileStatementNode &node)
{

    // Create Blocks
    auto check = BasicBlock::Create(builder_->getContext(), "check", builder_->GetInsertBlock()->getParent());
    auto loop = BasicBlock::Create(builder_->getContext(), "loop", builder_->GetInsertBlock()->getParent());
    auto tail = BasicBlock::Create(builder_->getContext(), "tail", builder_->GetInsertBlock()->getParent());

    // Jump into check
    builder_->CreateBr(check);

    // Create Loop Block
    builder_->SetInsertPoint(loop);
    visit(*node.get_statements());
    builder_->CreateBr(check);

    // Check Condition
    builder_->SetInsertPoint(check);

    visit(*node.get_expr());
    auto cond = value_;

    builder_->CreateCondBr(cond, loop, tail);

    // Tail
    builder_->SetInsertPoint(tail);
}

void CodeGenerator::visit(ModuleNode &node)
{

    variables_.beginScope();
    type_table_.beginScope();

    type_table_.insert("INTEGER", TypeInfoClass(INTEGER_TAG, {llvm::Type::getInt64Ty(ctx_)}, {}));
    type_table_.insert("BOOLEAN", TypeInfoClass(BOOLEAN_TAG, {llvm::Type::getInt1Ty(ctx_)}, {}));

    // define main
    auto main = module_->getOrInsertFunction("main", builder_->getInt32Ty());
    auto main_fct = cast<Function>(main.getCallee());
    auto entry = BasicBlock::Create(builder_->getContext(), "entry", main_fct);
    builder_->SetInsertPoint(entry);

    // global declarations
    create_declarations(*node.get_declarations(), true);

    // statements
    visit(*node.get_statements());

    variables_.endScope();
    type_table_.endScope();

    // return value
    builder_->CreateRet(builder_->getInt32(0));
    verifyFunction(*main_fct, &errs());
}

void CodeGenerator::emit()
{
    std::string ext;
    switch (output_type_)
    {
    case OutputFileType::AssemblyFile:
        ext = ".s";
        break;
    case OutputFileType::LLVMIRFile:
        ext = ".ll";
        break;
    default:
#if (defined(_WIN32) || defined(_WIN64)) && !defined(__MINGW32__)
        ext = ".obj";
#else
        ext = ".o";
#endif
        break;
    }
    std::string file = filename_ + ext;

    // serialize LLVM module to file
    std::error_code ec;
    // open an output stream with open flags "None"
    raw_fd_ostream output(file, ec, llvm::sys::fs::OF_None);
    if (ec)
    {
        std::cerr << ec.message() << std::endl;
        exit(ec.value());
    }
    if (output_type_ == OutputFileType::LLVMIRFile)
    {
        module_->print(output, nullptr);
        output.flush();
        return;
    }
    CodeGenFileType ft;

    switch (output_type_)
    {
    case OutputFileType::AssemblyFile:
#ifdef _LLVM_18
        ft = CodeGenFileType::AssemblyFile;
#else
        ft = CodeGenFileType::CGFT_AssemblyFile;
#endif
        break;
    default:
#ifdef _LLVM_18
        ft = CodeGenFileType::ObjectFile;
#else
        ft = CodeGenFileType::CGFT_ObjectFile;
#endif
        break;
    }

    legacy::PassManager pass;
    if (target_->addPassesToEmitFile(pass, output, nullptr, ft))
    {
        std::cerr << "Error: target machine cannot emit a file of this type." << std::endl;
        return;
    }
    pass.run(*module_);
    output.flush();
}

void CodeGenerator::generate_code(ModuleNode &node)
{

    visit(node);

    // verify module
    verifyModule(*module_, &errs());

    emit();
}
