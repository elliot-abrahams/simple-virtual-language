cmake_minimum_required(VERSION 3.20)
project(svm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GoogleTest)

# VM TESTS

set(VM_TESTS vm_tests)

set(VM_Sources
        vm/other/convTests.cpp
        vm/other/innTests.cpp
        vm/other/outTests.cpp

        vm/arithmetic/cgeTests.cpp
        vm/arithmetic/cgtTests.cpp
        vm/arithmetic/cleTests.cpp
        vm/arithmetic/cltTests.cpp
        vm/arithmetic/cneTests.cpp
        vm/arithmetic/ceqTests.cpp
        vm/arithmetic/sarTests.cpp
        vm/arithmetic/shrTests.cpp
        vm/arithmetic/shlTests.cpp
        vm/arithmetic/xorTests.cpp
        vm/arithmetic/orrTests.cpp
        vm/arithmetic/andTests.cpp
        vm/arithmetic/notTests.cpp
        vm/arithmetic/modTests.cpp
        vm/arithmetic/divTests.cpp
        vm/arithmetic/mulTests.cpp
        vm/arithmetic/subTests.cpp
        vm/arithmetic/addTests.cpp

        vm/control/jnzTests.cpp
        vm/control/jezTests.cpp
        vm/control/jmpTests.cpp
        vm/control/callRetTests.cpp

        vm/memory/allocFreeTests.cpp
        vm/memory/loadLStoreLTests.cpp
        vm/memory/loadGStoreGTests.cpp
        vm/memory/loadBStoreBTests.cpp
        vm/memory/loadStoreTests.cpp

        vm/Stack/swapTests.cpp
        vm/Stack/dupTests.cpp
        vm/Stack/popTests.cpp
        vm/Stack/pushTests.cpp

        vm/core/haltTests.cpp

        vm/vmTestUtils.h
        vm/vmTests.cpp
)

add_executable(${VM_TESTS} ${VM_Sources})

target_link_libraries(${VM_TESTS} PRIVATE
    gtest_main
    svm_lib
)

gtest_discover_tests(${VM_TESTS})


# PARSER TESTS

set(PARSER_TESTS parser_tests)

set(Parser_Sources
        parser/functionDeclTests.cpp

        parser/statementTests/returnStatementTests.cpp
        parser/statementTests/FunctionCallStatementTests.cpp
        parser/statementTests/breakStatementTests.cpp
        parser/statementTests/continueStatementTests.cpp
        parser/statementTests/whileStatementTests.cpp
        parser/statementTests/ifStatementTests.cpp
        parser/statementTests/assignmentTests.cpp
        parser/statementTests/VarDeclTests.cpp
        parser/statementTests/blockTests.cpp

        parser/expressionTests/precedenceTests.cpp
        parser/expressionTests/binaryExpressionTests.cpp
        parser/expressionTests/unaryExpressionTests.cpp
        parser/expressionTests/functionCallExpressionTests.cpp

        parser/parserTestutils.h
        parser/parserTests.cpp
)

add_executable(${PARSER_TESTS} ${Parser_Sources})

target_link_libraries(${PARSER_TESTS} PRIVATE
        gtest_main
        svm_lib
)

gtest_discover_tests(${PARSER_TESTS})


# TOKENISER TESTS

set(TOKENISER_TESTS tokeniser_tests)

set(Tokeniser_Sources
        tokeniser/invalidTokenTests.cpp
        tokeniser/tokenSequenceTests.cpp
        tokeniser/identifierTests.cpp
        tokeniser/keywordTests.cpp
        tokeniser/literalTests.cpp
        tokeniser/SymbolTests.cpp

        tokeniser/tokeniserTestUtils.h
        tokeniser/tokeniserTests.cpp
)

add_executable(${TOKENISER_TESTS} ${Tokeniser_Sources})

target_link_libraries(${TOKENISER_TESTS} PRIVATE
        gtest_main
        svm_lib
)

gtest_discover_tests(${TOKENISER_TESTS})


# TOKENISER TESTS

set(SEMANTIC_TESTS semantic_tests)

set(Semantic_Sources
        semantic/functionTests.cpp
        semantic/variableAndScopeTests.cpp

        semantic/statementTests/returnStatementTests.cpp
        semantic/statementTests/functionCallStatementTests.cpp
        semantic/statementTests/breakStatementTests.cpp
        semantic/statementTests/continueStatementTests.cpp
        semantic/statementTests/whileStatementTests.cpp
        semantic/statementTests/ifStatementTests.cpp
        semantic/statementTests/AssignmentTests.cpp
        semantic/statementTests/varDeclTests.cpp

        semantic/expressionTests/binaryExpressionTests.cpp
        semantic/expressionTests/unaryExpressionTests.cpp
        semantic/expressionTests/functionCallExpressionTests.cpp

        semantic/semanticTestUtils.h
        semantic/semanticTests.cpp
)

add_executable(${SEMANTIC_TESTS} ${Semantic_Sources})

target_link_libraries(${SEMANTIC_TESTS} PRIVATE
        gtest_main
        svm_lib
)

gtest_discover_tests(${SEMANTIC_TESTS})
