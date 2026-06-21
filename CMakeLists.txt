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
        src/include/ISA.h
)

add_library(${This} STATIC ${Sources} ${Headers})

add_executable(svm src/main.cpp)

target_link_libraries(svm
    PRIVATE
    ${This}
)

add_subdirectory(tests)