cmake_minimum_required(VERSION 3.20)
project(sv)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GoogleTest)

# VM TESTS

set(VM_TESTS vm_tests)

set(VM_Sources
        vm/other/ConvTests.cpp

        vm/arithmetic/CgeTests.cpp
        vm/arithmetic/CgtTests.cpp
        vm/arithmetic/CleTests.cpp
        vm/arithmetic/CltTests.cpp
        vm/arithmetic/CneTests.cpp
        vm/arithmetic/CeqTests.cpp
        vm/arithmetic/SarTests.cpp
        vm/arithmetic/ShrTests.cpp
        vm/arithmetic/ShlTests.cpp
        vm/arithmetic/XorTests.cpp
        vm/arithmetic/OrrTests.cpp
        vm/arithmetic/AndTests.cpp
        vm/arithmetic/NotTests.cpp
        vm/arithmetic/ModTests.cpp
        vm/arithmetic/DivTests.cpp
        vm/arithmetic/MulTests.cpp
        vm/arithmetic/SubTests.cpp
        vm/arithmetic/AddTests.cpp

        vm/control/JnzTests.cpp
        vm/control/JezTests.cpp
        vm/control/JmpTests.cpp
        vm/control/CallRetTests.cpp
        vm/control/NativeTests.cpp

        vm/memory/AllocFreeTests.cpp
        vm/memory/LoadLStoreLTests.cpp
        vm/memory/LoadGStoreGTests.cpp
        vm/memory/LoadBStoreBTests.cpp
        vm/memory/LoadStoreTests.cpp

        vm/stack/SwapTests.cpp
        vm/stack/DupTests.cpp
        vm/stack/PopTests.cpp
        vm/stack/PushTests.cpp

        vm/core/HaltTests.cpp

        vm/VmTestUtils.h
        vm/VmTests.cpp
)

add_executable(${VM_TESTS} ${VM_Sources})

target_link_libraries(${VM_TESTS} PRIVATE
    gtest_main
    sv_lib
)

gtest_discover_tests(${VM_TESTS})


# PARSER TESTS

set(PARSER_TESTS parser_tests)

set(Parser_Sources
        parser/FunctionDeclTests.cpp

        parser/statementTests/ReturnStatementTests.cpp
        parser/statementTests/FunctionCallStatementTests.cpp
        parser/statementTests/BreakStatementTests.cpp
        parser/statementTests/ContinueStatementTests.cpp
        parser/statementTests/WhileStatementTests.cpp
        parser/statementTests/IfStatementTests.cpp
        parser/statementTests/AssignmentTests.cpp
        parser/statementTests/VarDeclTests.cpp
        parser/statementTests/BlockTests.cpp

        parser/expressionTests/PrecedenceTests.cpp
        parser/expressionTests/BinaryExpressionTests.cpp
        parser/expressionTests/UnaryExpressionTests.cpp
        parser/expressionTests/FunctionCallExpressionTests.cpp

        parser/ParserTestUtils.h
        parser/ParserTests.cpp
)

add_executable(${PARSER_TESTS} ${Parser_Sources})

target_link_libraries(${PARSER_TESTS} PRIVATE
        gtest_main
        sv_lib
)

gtest_discover_tests(${PARSER_TESTS})


# TOKENISER TESTS

set(TOKENISER_TESTS tokeniser_tests)

set(Tokeniser_Sources
        tokeniser/InvalidTokenTests.cpp
        tokeniser/TokenSequenceTests.cpp
        tokeniser/IdentifierTests.cpp
        tokeniser/KeywordTests.cpp
        tokeniser/LiteralTests.cpp
        tokeniser/SymbolTests.cpp

        tokeniser/TokeniserTestUtils.h
        tokeniser/TokeniserTests.cpp
)

add_executable(${TOKENISER_TESTS} ${Tokeniser_Sources})

target_link_libraries(${TOKENISER_TESTS} PRIVATE
        gtest_main
        sv_lib
)

gtest_discover_tests(${TOKENISER_TESTS})


# SEMANTIC TESTS

set(SEMANTIC_TESTS semantic_tests)

set(Semantic_Sources
        semantic/FunctionTests.cpp
        semantic/VariableAndScopeTests.cpp

        semantic/statementTests/ReturnStatementTests.cpp
        semantic/statementTests/FunctionCallStatementTests.cpp
        semantic/statementTests/BreakStatementTests.cpp
        semantic/statementTests/ContinueStatementTests.cpp
        semantic/statementTests/WhileStatementTests.cpp
        semantic/statementTests/IfStatementTests.cpp
        semantic/statementTests/AssignmentTests.cpp
        semantic/statementTests/VarDeclTests.cpp

        semantic/expressionTests/BinaryExpressionTests.cpp
        semantic/expressionTests/UnaryExpressionTests.cpp
        semantic/expressionTests/FunctionCallExpressionTests.cpp

        semantic/SemanticTestUtils.h
        semantic/SemanticTests.cpp
)

add_executable(${SEMANTIC_TESTS} ${Semantic_Sources})

target_link_libraries(${SEMANTIC_TESTS} PRIVATE
        gtest_main
        sv_lib
)

gtest_discover_tests(${SEMANTIC_TESTS})

# INTEGRATION TESTS

set(INTEGRATION_TESTS integration_tests)

set(Integration_Sources
        integration/FunctionTests.cpp

        integration/statementTests/WhileStatementTests.cpp
        integration/statementTests/IfStatementTests.cpp
        integration/statementTests/AssignmentTests.cpp

        integration/expressionTests/PrecedenceTests.cpp
        integration/expressionTests/BinaryExpressionTests.cpp
        integration/expressionTests/UnaryExpressionTests.cpp

        integration/IntegrationTestUtils.h
        integration/IntegrationTests.cpp
        integration/ScopeTests.cpp
)

add_executable(${INTEGRATION_TESTS} ${Integration_Sources})

target_link_libraries(${INTEGRATION_TESTS} PRIVATE
        gtest_main
        sv_lib
)

gtest_discover_tests(${INTEGRATION_TESTS})
