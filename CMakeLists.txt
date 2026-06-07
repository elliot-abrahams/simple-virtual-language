cmake_minimum_required(VERSION 3.20)
project(svm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(svm
        "src/assembler/Lexer.cpp"
        "src/assembler/Lexer.h"
        "src/assembler/Assembler.cpp"
        "src/assembler/Assembler.h"
        "src/assembler/Parser.cpp"
        "src/assembler/Parser.h"
        "src/assembler/AssemblerDefs.h"
        "src/main.cpp"
        "src/vm/VM.cpp"
        "src/vm/VM.h"
        "src/vm/memory/MemoryManager.cpp"
        "src/vm/memory/MemoryManager.h"
        "src/vm/OperandStack.cpp"
        "src/vm/OperandStack.h"
        "src/vm/VMError.h"
        "src/Driver.cpp"
        "src/Driver.h"
)