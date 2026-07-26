cmake_minimum_required(VERSION 3.20)
project(svm)

set(This svm_lib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(googletest)

set(Headers
    src/assembler/Lexer.h
    src/assembler/Parser.h
    src/assembler/Assembler.h
    src/include/AssemblerDefs.h
    src/vm/VM.h
    src/vm/memory/MemoryManager.h
    src/vm/OperandStack.h
    src/vm/VMError.h
    src/Driver.h
    src/vm/ArithmeticOps.h
    src/vm/TypeConversions.h
    src/include/ISA.h
    src/vm/memory/CallStackManager.h
    src/vm/memory/HeapManager.h
    src/compiler/Compiler.h
    src/compiler/Token.h
    src/compiler/Tokeniser.h
    src/compiler/Token.h
    src/compiler/Parser.h
    src/compiler/AST.h
    src/include/Error.h
    src/compiler/SymbolTable.h
    src/compiler/SemanticAnalyser.h
    src/compiler/CodeGenerator.h
    src/compiler/LanguageTypes.h
)

set(Sources
    src/assembler/Lexer.cpp
    src/assembler/Parser.cpp
    src/assembler/Assembler.cpp
    src/vm/VM.cpp
    src/vm/memory/MemoryManager.cpp
    src/vm/OperandStack.cpp
    src/Driver.cpp
    src/vm/ArithmeticOps.cpp
    src/vm/TypeConversions.cpp
    src/vm/memory/CallStackManager.cpp
    src/vm/memory/HeapManager.cpp
    src/compiler/Compiler.cpp
    src/compiler/Tokeniser.cpp
    src/compiler/Parser.cpp
    src/compiler/SymbolTable.cpp
    src/compiler/SemanticAnalyser.cpp
    src/compiler/CodeGenerator.cpp
        src/compiler/BuiltinFunctions.cpp
        src/compiler/BuiltinFunctions.h
)

add_library(${This} STATIC ${Sources} ${Headers})

add_executable(svm src/main.cpp)

target_link_libraries(svm
    PRIVATE
    ${This}
)

add_subdirectory(tests)