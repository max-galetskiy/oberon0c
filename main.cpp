/*
 * Main class of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/14/17.
 */

#include <iostream>
#include <string>
#include "scanner/Scanner.h"
#include "parser/Parser.h"
#include "semantic_checker/SemanticChecker.h"
#include "code_generator/CodeGenerator.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;

int main(const int argc, const char *argv[]) {

    const auto usage_string = "Usage: oberon0c <filename> [-o|-s|-ll] [--debug]";

    if (argc < 2 || argc > 4) {
        cerr << usage_string << endl;
        exit(1);
    }
    string filename = argv[1];
    Logger logger;

    OutputFileType output_type;
    if(argc > 2){
        auto flag = string(argv[2]);
        if(flag == "-o" || flag == "-O"){
            output_type = OutputFileType::ObjectFile;
        }
        else if(flag == "-s" || flag == "-S"){
            output_type = OutputFileType::AssemblyFile;
        }
        else if(flag == "-ll" || flag == "-LL"){
            output_type = OutputFileType::LLVMIRFile;
        }
        else{
            cerr << "Invalid argument: " << argv[2] << std::endl << usage_string << std::endl;
            exit(1);
        }
    }
    else{
        output_type = OutputFileType::LLVMIRFile;
    }

    if(argc > 3){
        if(string(argv[3]) == "--debug"){
            logger.setLevel(LogLevel::DEBUG);
        }
        else{
            cerr << "Invalid argument: " << argv[3] << std::endl << usage_string << std::endl;
            exit(1);
        }
    }
    else{
        logger.setLevel(LogLevel::INFO);
    }



    // Scanning
    Scanner scanner(filename, logger);

    // Parsing
    Parser parser(scanner,logger);
    auto ast = parser.parse();
    if(ast && logger.getErrorCount() == 0){
        std::cout << "Compiled Program:" << std::endl << *ast << std::endl;
        logger.info("Parsing successful.");

        // Semantic Checking
        SemanticChecker semantics(logger);
        semantics.validate_program(*ast);

        if(logger.getErrorCount() > 0){
            logger.info("Errors occurred during semantic checking.");
        }
        else{

            logger.info("Semantic checking successful. Starting code generation...");

            // Code Generation
            CodeGenerator code_gen(filename,output_type);
            code_gen.generate_code(*ast);

            logger.info("Code generation successful.");

        }

    }else{
        logger.info("Errors occurred during parsing.");
    }

    string status = (logger.getErrorCount() == 0 ? "complete" : "failed");
    logger.info("Compilation " + status + ": " +
                to_string(logger.getErrorCount()) + " error(s), " +
                to_string(logger.getWarningCount()) + " warning(s), " +
                to_string(logger.getInfoCount()) + " message(s).");
    exit(logger.getErrorCount() != 0);
}