//
// Created by M on 19.01.2025.
//

#include <utility>
#include "CodeGenerator.h"
#include "util/panic.h"

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
        panic(error);
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

    // Boolean operators are treated specially because of short-circuit evalution
    if(op == SourceOperator::AND || op == SourceOperator::OR){

        bool is_and_op = op == SourceOperator::AND;

        // LHS will always be evaluated
        auto lhs_block = builder_->GetInsertBlock();
        expr.get_lhs()->accept(*this);
        auto lhs_value = value_;

        auto bool_rhs = BasicBlock::Create(builder_->getContext(),"bool_rhs",builder_->GetInsertBlock()->getParent());
        auto post_bool = BasicBlock::Create(builder_->getContext(),"post_bool",builder_->GetInsertBlock()->getParent());

        (is_and_op)? builder_->CreateCondBr(lhs_value,bool_rhs,post_bool) : builder_->CreateCondBr(lhs_value,post_bool,bool_rhs);             // Branching dependent on boolean operation

        // RHS is executed in bool_rhs block
        builder_->SetInsertPoint(bool_rhs);
        expr.get_rhs()->accept(*this);
        auto rhs_value = value_;
        auto rhs_block = builder_->GetInsertBlock();       // Note that for nested conditions with further basic blocks this may be different than "bool_rhs"
        builder_->CreateBr(post_bool);

        // Rest of the code is continued in the post_bool branch after resolving the SSA-Phi function
        builder_->SetInsertPoint(post_bool);
        auto phi = builder_->CreatePHI(value_->getType(),2,"phi");
        phi->addIncoming(lhs_value,lhs_block);
        phi->addIncoming(rhs_value,rhs_block);
        value_ = phi;
        return;
    }


    expr.get_lhs()->accept(*this);
    llvm::Value *lhsValue = value_;

    expr.get_rhs()->accept(*this);
    llvm::Value *rhsValue = value_;

    switch (op)
    {

    // Arithmetic Operator
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

    // Note: Boolean operators already handled

    // Relational operator
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
    LoadIdentSelector(*ident, node.get_selector());
}

void CodeGenerator::LoadIdentSelector(IdentNode &ident, SelectorNode *selector, bool return_pointer)
{
    std::string name = ident.get_value();
    llvm::Value *var = variables_.lookup(name)->value;
    TypeInfo *type = ident.get_actual_type().get();

    if (!selector || selector->get_selector()->empty())
    {
        LoadIdent(ident, return_pointer);
        return;
    }

    auto selectors = *(selector->get_selector());

    assert(!selectors.empty());
    llvm::Value *zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), 0);

    for (auto & tuple_ref : selectors)
    {
        auto &[is_array, ident_ptr, expr_ptr] = tuple_ref;

        if (!is_array) // Record field access
        {
            assert(type->tag == TypeTag::RECORD);
            auto record_fields = std::get<RecordTypeInfo>(type->extended_info.value()).fields;

            assert(ident_ptr);
            std::string field_name = ident_ptr->get_value();

            // Find the field index
            size_t field_index = std::distance(record_fields.begin(), record_fields.find(field_name));
            assert(field_index < 0 && "Field not found in record");
            auto idx = static_cast<uint64_t>(field_index);

            llvm::Value *field_index_val = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx_), idx);
            type = record_fields[field_name].get();
            var = builder_->CreateGEP(variables_.lookup_type(type->name), var, {zero, field_index_val}, "rec_field_" + field_name);

        }
        else // Array access
        {
            assert(type->tag == TypeTag::ARRAY);
            assert(!ident_ptr);

            expr_ptr->accept(*this);
            llvm::Value *index_val = value_;
            llvm::Type  *arr_type = variables_.lookup_type(type->name);

            var = builder_->CreateGEP(arr_type, var, {zero, index_val}, "arr_ptr_" + name);
            type = std::get<ArrayTypeInfo>(type->extended_info.value()).elementType.get();


        }
    }

    if (return_pointer)
    {
        value_ = var;
    }
    else
    {
        llvm::Value *val = builder_->CreateLoad(variables_.lookup_type(type->name), var, "load_" + name);
        value_ = val;
    }
}

void CodeGenerator::visit(IdentNode &ident)
{
    LoadIdent(ident, false);
}

void CodeGenerator::LoadIdent(IdentNode &ident, bool return_pointer)
{
    std::string name = ident.get_value();

    auto ident_info = variables_.lookup(ident.get_value());

    llvm::Value *var = ident_info->value;
    TypeInfo *type = ident.get_actual_type().get();
    bool is_pointer = ident_info->is_pointer;

    if (!var->getType()->isPointerTy() || return_pointer)
    {
        value_ = var;
    }
    else
    {
        auto llvm_type = variables_.lookup_type(type->name);

        if (is_pointer)
        {
            auto val = builder_->CreateLoad(llvm_type->getPointerTo(), var, "load_" + name);
            auto deref = builder_->CreateLoad(llvm_type, val, "deref_" + name);
            value_ = deref;
        }
        else
        {
            llvm::Value *val = builder_->CreateLoad(llvm_type, var, "load_" + name);
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
    panic("Unreachable, this should be handled in IdentSelectorExpressionNode");
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
        variables_.insert_type(name, create_llvm_type(*it->second));
    }

    auto constants = node.get_constants();
    for (auto it = begin(constants); it != end(constants); ++it)
    {

        auto ident = it->first;
        std::string name = ident->get_value();

        auto type = it->first->get_actual_type();
        auto llvm_type = variables_.lookup_type(type->name);

        it->second->accept(*this);
        llvm::Value *value = value_;
        llvm::Value *var;
        if (is_global)
        {
            var = new GlobalVariable(*module_, llvm_type, false, GlobalValue::InternalLinkage, dyn_cast<Constant>(value), name);
        }
        else
        {
            var = builder_->CreateAlloca(llvm_type, nullptr, name);
            builder_->CreateStore(value, var);
        }

        variables_.insert(name,var,false);
    }

    auto variables = node.get_variables();
    for (auto it = begin(variables); it != end(variables); ++it)
    {
        for (auto ident_itr = it->first.begin(); ident_itr != it->first.end(); ident_itr++)
        {
            auto ident = *ident_itr;
            auto type = ident->get_actual_type();
            auto llvm_type = variables_.lookup_type(type->name);
            std::string name = ident->get_value();

            llvm::Value *var;
            if (is_global)
            {
                var = new GlobalVariable(*module_, llvm_type, false, GlobalValue::InternalLinkage, Constant::getNullValue(llvm_type), name);
            }
            else
            {
                var = builder_->CreateAlloca(llvm_type, nullptr, name);
            }

            variables_.insert(name,var, false);
        }
    }

    auto procedures = node.get_procedures();
    for (auto itr = procedures.begin(); itr != procedures.end(); itr++)
    {
        visit(**itr);
    }
}

void CodeGenerator::visit(TypeNode &node){(void)node;}
void CodeGenerator::visit(ArrayTypeNode &node) {(void)node;}
void CodeGenerator::visit(RecordTypeNode &node){(void)node;}

llvm::Type *CodeGenerator::create_llvm_type(TypeNode &node) {

    if(node.getNodeType() == NodeType::ident){
        auto ident_name = dynamic_cast<IdentNode&>(node).get_value();
        return variables_.lookup_type(ident_name);
    }
    else if(node.getNodeType() == NodeType::array_type){
        auto array_type_node = &dynamic_cast<ArrayTypeNode&>(node);

        auto elem_type = create_llvm_type(*array_type_node->get_type_node());
        auto dim = array_type_node->get_dim().value();

        return llvm::ArrayType::get(elem_type, static_cast<long unsigned int>(dim));

    }
    else if(node.getNodeType() == NodeType::record_type){
        auto record_type_node = &dynamic_cast<RecordTypeNode&>(node);
        auto raw_fields = record_type_node->get_fields();
        std::vector<llvm::Type *> llvm_fields;

        for(auto field_section: raw_fields){

            auto field_type_llvm = create_llvm_type(*field_section.second);
            for(auto field: field_section.first){
                llvm_fields.push_back(field_type_llvm);
            }

        }

        return llvm::StructType::create(ctx_, llvm_fields);

    }

    panic("unreachable (invalid TypeNode type)");

}

void CodeGenerator::visit(ProcedureDeclarationNode &node)
{
    auto prev_block = builder_->GetInsertBlock();

    auto name = node.get_names().first->get_value();
    auto arguments = node.get_parameters();
    auto return_type = node.get_return_type_node();

    variables_.beginScope();

    // Create Signature
    std::vector<Type *> llvm_params;
    Type* llvm_return_type = builder_->getVoidTy();

    if (arguments)
    {
        for (auto itr = arguments->begin(); itr != arguments->end(); itr++)
        {

            bool is_var = std::get<0>(**itr);
            auto idents = std::get<1>(**itr).get();
            auto typenode = std::get<2>(**itr).get();
            auto llvm_type = create_llvm_type(*typenode);

            if (is_var)
            {
                llvm_type = llvm_type->getPointerTo();
            }

            for (auto param = idents->begin(); param != idents->end(); param++)
            {
                llvm_params.push_back(llvm_type);
            }
        }
    }

    if(return_type){
        llvm_return_type = create_llvm_type(*return_type);
    }

    auto signature = FunctionType::get(llvm_return_type, llvm_params, false);

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

                auto arg_type = param->get()->get_actual_type();
                auto llvm_type = variables_.lookup_type(arg_type->name);

                // Reserve place for argument on the stack
                auto param_type = (is_var)? llvm_type->getPointerTo() : llvm_type;
                auto param_value = builder_->CreateAlloca(param_type, nullptr,param->get()->get_value());
                builder_->CreateStore(arg_itr,param_value);

                variables_.insert(param->get()->get_value(),param_value,is_var);
                arg_itr->setName(param->get()->get_value());
                arg_itr++;
            }
        }
    }

    // Add Procedure Declaration and Statements
    visit(*node.get_declarations());
    visit(*node.get_statements());

    variables_.endScope();

    // Add Return in case of void type
    if(llvm_return_type == builder_->getVoidTy()){
        builder_->CreateRetVoid();
    }
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
    case NodeType::return_statement:
        visit(dynamic_cast<ReturnStatementNode &>(node));
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

void CodeGenerator::visit(ReturnStatementNode &node) {

    if(!node.get_value()){
        builder_->CreateRetVoid();
    }

    visit(*node.get_value());
    builder_->CreateRet(value_);

}

void CodeGenerator::visit(ModuleNode &node)
{

    variables_.beginScope();
    variables_.insert_type("INTEGER",llvm::Type::getInt64Ty(ctx_));
    variables_.insert_type("BOOLEAN",llvm::Type::getInt1Ty(ctx_));

    // define main
    auto main = module_->getOrInsertFunction("main", builder_->getInt64Ty());
    auto main_fct = cast<Function>(main.getCallee());
    auto entry = BasicBlock::Create(builder_->getContext(), "entry", main_fct);
    builder_->SetInsertPoint(entry);

    // global declarations
    create_declarations(*node.get_declarations(), true);

    // statements
    visit(*node.get_statements());

    variables_.endScope();

    // return value
    builder_->CreateRet(builder_->getInt64(0));
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
