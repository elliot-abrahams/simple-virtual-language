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
        Parser/statementTests/returnStatementTests.cpp
        Parser/statementTests/FunctionCallStatementTests.cpp
        Parser/statementTests/breakStatementTests.cpp
        Parser/statementTests/continueStatementTests.cpp
        Parser/statementTests/whileStatementTests.cpp
        Parser/statementTests/ifStatementTests.cpp
        Parser/statementTests/assignmentTests.cpp
        Parser/statementTests/VarDeclTests.cpp
        Parser/statementTests/blockTests.cpp

        Parser/expressionTests/precedenceTests.cpp
        Parser/expressionTests/binaryExpressionTests.cpp
        Parser/expressionTests/unaryExpressionTests.cpp
        Parser/expressionTests/functionCallExpressionTests.cpp

        Parser/parserTestutils.h
        Parser/parserTests.cpp
        Parser/functionDeclTests.cpp
)

add_executable(${PARSER_TESTS} ${Parser_Sources})

target_link_libraries(${PARSER_TESTS} PRIVATE
        gtest_main
        svm_lib
)

gtest_discover_tests(${PARSER_TESTS})
