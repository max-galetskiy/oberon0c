/*
 * Main class of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/14/17.
 */

#include <iostream>
#include <string>
#include "util/command_line_options.hpp"
#include "scanner/Scanner.h"
#include "parser/Parser.h"
#include "semantic_checker/SemanticChecker.h"
#include "code_generator/CodeGenerator.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;

int main(const int argc, const char *argv[]) {

    auto options = init_options();
    auto vm = create_value_map(options.get(),argc,argv);
    po::notify(*vm);

    if(vm->count("help")){
        cout << "Oberon0 to LLVM-IR Compiler\n";
        cout << "Usage: oberon0c [options] file\n";
        cout << *options << std::endl;
        return 0;
    }

    if(!vm->count("input")){
        cerr << "No input files specified." << endl;
        return 1;
    }

    string filename = (*vm)["input"].as<string>();

    Logger logger;
    if(vm->count("quiet")){
        logger.setLevel(LogLevel::QUIET);
    } else if(vm->count("debug")){
        logger.setLevel(LogLevel::DEBUG);
    } else{
        logger.setLevel(LogLevel::INFO);
    }

    OutputFileType output_type;
    auto specified_output_type = (vm->count("filetype"))? (*vm)["filetype"].as<string>() : "ll";
    if(specified_output_type == "ll"){
        output_type = OutputFileType::LLVMIRFile;
    }else if(specified_output_type == "asm"){
        output_type = OutputFileType::AssemblyFile;
    }else if(specified_output_type == "obj"){
        output_type = OutputFileType::ObjectFile;
    }else{
        cerr << "Requested output filetype '" << specified_output_type << "' not supported." << endl;
        return 1;
    }

    // Scanning
    logger.info("Starting scanning...",true);
    Scanner scanner(filename, logger);
    logger.info("Scanning successful. Starting Parsing...",true);

    // Parsing
    Parser parser(scanner,logger);
    auto ast = parser.parse();
    if(ast && logger.getErrorCount() == 0){

        if(vm->count("print")){
            std::cout << "Compiled Program:" << std::endl << *ast << std::endl;
        }

        logger.info("Parsing successful. Starting semantic checking...", true);

        // Semantic Checking
        SemanticChecker semantics(logger);
        semantics.validate_program(*ast);

        if(logger.getErrorCount() > 0){
            logger.info("Errors occurred during semantic checking.", true);
        }
        else{

            logger.info("Semantic checking successful. Starting code generation...", true);

            // Code Generation
            CodeGenerator code_gen(filename,output_type);
            code_gen.generate_code(*ast);

            logger.info("Code generation successful.", true);

        }

    }else{
        logger.info("Errors occurred during parsing.", true);
    }

    string status = (logger.getErrorCount() == 0 ? "complete" : "failed");
    logger.info("Compilation " + status + ": " +
                to_string(logger.getErrorCount()) + " error(s), " +
                to_string(logger.getWarningCount()) + " warning(s), " +
                to_string(logger.getInfoCount()) + " message(s).", true);
    exit(logger.getErrorCount() != 0);
}