cmake_minimum_required(VERSION 3.28)

set(OBERON0_C "oberon0c")
project(${OBERON0_C} VERSION 0.0.1)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # warning level 4
    add_compile_options(/Wall /W4 /external:anglebrackets /external:W0)
else ()
    # lots of warnings and all warnings as errors -Wglobal-constructors -Wexit-time-destructors
    add_compile_options(-W -Wall -Wextra -Wpedantic -Wno-error=deprecated -Wunreachable-code -Winvalid-pch -Wcast-align -Wformat=2 -Wformat-nonliteral -Wmissing-declarations -Wmissing-noreturn -Wmissing-format-attribute -Wmissing-include-dirs -Wredundant-decls -Wswitch-default -Wsign-conversion)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-Wweak-vtables)
        if (WIN32)
            add_compile_options(-fms-extensions)
        endif ()
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # using GCC
    endif ()
endif ()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# make Boost work with CMake >= 3.30.0
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.30.0")
    cmake_policy(SET CMP0167 NEW)
endif ()

# define DEBUG macro
if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_DEBUG)
endif ()

# add the cmake folder of the project for extension modules
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(Boost_USE_MULTITHREADED ON)
set(Boost_NO_WARN_NEW_VERSIONS 1)

# add source tree to search path to avoid relative includes
include_directories(BEFORE .)

set(UTIL_SOURCES
        util/Logger.cpp util/Logger.h
        util/panic.cpp util/panic.h
)

set(SCANNER_SOURCES
        scanner/Scanner.cpp scanner/Scanner.h
        scanner/Token.cpp scanner/Token.h
        scanner/LiteralToken.cpp scanner/LiteralToken.h
        scanner/IdentToken.cpp scanner/IdentToken.h
        scanner/UndefinedToken.cpp scanner/UndefinedToken.h)

set(PARSER_SOURCES
        parser/Parser.cpp parser/Parser.h
        util/datastructures/ast/Node.cpp util/datastructures/ast/Node.h
        util/datastructures/ast/NodeVisitor.cpp util/datastructures/ast/NodeVisitor.h
        util/datastructures/ast/base_blocks/IdentNode.cpp util/datastructures/ast/base_blocks/IdentNode.h
        util/datastructures/ast/base_blocks/IntNode.cpp util/datastructures/ast/base_blocks/IntNode.h
        util/datastructures/ast/base_blocks/SelectorNode.cpp util/datastructures/ast/base_blocks/SelectorNode.h
        util/datastructures/ast/base_blocks/ExpressionNode.cpp util/datastructures/ast/base_blocks/ExpressionNode.h

        util/datastructures/ast/statements/StatementNode.cpp util/datastructures/ast/statements/StatementNode.h
        util/datastructures/ast/statements/AssignmentNode.cpp util/datastructures/ast/statements/AssignmentNode.h
        util/datastructures/ast/statements/ProcedureCallNode.cpp util/datastructures/ast/statements/ProcedureCallNode.h
        util/datastructures/ast/statements/RepeatStatementNode.cpp util/datastructures/ast/statements/RepeatStatementNode.h
        util/datastructures/ast/statements/WhileStatementNode.cpp util/datastructures/ast/statements/WhileStatementNode.h
        util/datastructures/ast/statements/IfStatementNode.cpp util/datastructures/ast/statements/IfStatementNode.h
        util/datastructures/ast/statements/StatementSequenceNode.cpp util/datastructures/ast/statements/StatementSequenceNode.h

        util/datastructures/ast/declarations/TypeNode.cpp util/datastructures/ast/declarations/TypeNode.h
        util/datastructures/ast/declarations/ArrayTypeNode.cpp util/datastructures/ast/declarations/ArrayTypeNode.h
        util/datastructures/ast/declarations/RecordTypeNode.cpp util/datastructures/ast/declarations/RecordTypeNode.h
        util/datastructures/ast/declarations/ProcedureDeclarationNode.cpp util/datastructures/ast/declarations/ProcedureDeclarationNode.h
        util/datastructures/ast/declarations/DeclarationsNode.cpp util/datastructures/ast/declarations/DeclarationsNode.h

        util/datastructures/ast/ModuleNode.cpp util/datastructures/ast/ModuleNode.h

)

set(SEMANTIC_SOURCES
        util/datastructures/tables/SymbolTable.cpp util/datastructures/tables/SymbolTable.h
        util/datastructures/tables/ScopeTable.cpp  util/datastructures/tables/ScopeTable.h
        semantic_checker/SemanticChecker.cpp semantic_checker/SemanticChecker.h
)

set(CODEGEN_SOURCES
        code_generator/CodeGenerator.cpp
        code_generator/CodeGenerator.h
        util/datastructures/tables/TypeInfoTable.cpp
        util/datastructures/tables/TypeInfoTable.h
        util/datastructures/tables/VariableTable.cpp
        util/datastructures/tables/VariableTable.h
)

set(ALL_SOURCES global.h ${UTIL_SOURCES} ${SCANNER_SOURCES} ${PARSER_SOURCES} ${SEMANTIC_SOURCES} ${CODEGEN_SOURCES}
)

add_executable(${OBERON0_C} main.cpp ${ALL_SOURCES})

find_package(Boost REQUIRED COMPONENTS program_options)
if (Boost_FOUND)
    message(STATUS "Found Boost: ${Boost_VERSION}")
    target_link_libraries(${OBERON0_C} PRIVATE Boost::headers Boost::program_options)
endif ()

find_package(LLVM REQUIRED CONFIG)
if (LLVM_FOUND)
    if (${LLVM_VERSION} LESS 16.0.0)
        message(STATUS "Warning: found legacy LLVM: ${LLVM_VERSION}")
        add_definitions(-D_LLVM_LEGACY)
    else ()
        message(STATUS "Found LLVM: ${LLVM_VERSION}")
        if (${LLVM_VERSION} MATCHES "16.*.*")
            add_definitions(-D_LLVM_16)
        elseif (${LLVM_VERSION} MATCHES "17.*.*")
            add_definitions(-D_LLVM_17)
        elseif (${LLVM_VERSION} MATCHES "18.*.*")
            add_definitions(-D_LLVM_18)
        endif()
    endif ()
    add_definitions(${LLVM_DEFINITIONS})
    include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
    llvm_map_components_to_libnames(llvm_libs core support passes ${LLVM_TARGETS_TO_BUILD})
    target_link_libraries(${OBERON0_C} PRIVATE ${llvm_libs})
endif ()

configure_file(config.h.in config.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})