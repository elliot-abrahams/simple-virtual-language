#ifndef SVM_VMTESTUTILS_H
#define SVM_VMTESTUTILS_H

#include "../../src/include/ISA.h"
#include "gtest/gtest.h"
#include "../../src/assembler/Assembler.h"
#include "../../src/vm/VM.h"

template<typename T>
void EXPECT_OPERAND_VM_STACK_EQ(
    const std::string& source,
    const ISA::Type expectedType,
    const T expectedValue
) {
    try {
        Assembler assembler;
        VM vm;

        const auto bytecode = assembler.assembleString(source);

        vm.run(&bytecode.value());
        const Value result = vm.popOperandStack();

        EXPECT_EQ(result.type, expectedType);
        auto resultVal = std::get<T>(result.toTyped());
        EXPECT_EQ(resultVal, expectedValue);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

template<typename T>
void EXPECT_N_OPERANDS_OF_SAME_TYPE_VM_STACK_EQ(
    const std::string& source,
    const ISA::Type expectedType,
    const std::vector<T> expectedValues
) {
    try {
        Assembler assembler;
        VM vm;

        const auto bytecode = assembler.assembleString(source);

        vm.run(&bytecode.value());

        for (int i = 0; i < expectedValues.size(); i++) {
            const Value result = vm.popOperandStack();
            EXPECT_EQ(result.type, expectedType);
            auto resultVal = std::get<T>(result.toTyped());
            EXPECT_EQ(resultVal, expectedValues.at(i));
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

inline void EXPECT_VM_ERROR(const std::string& source) {
    Assembler assembler;
    VM vm;

    const auto bytecode = assembler.assembleString(source);

    EXPECT_THROW({
        vm.run(&bytecode.value());
    }, VMError);
}

template<typename T>
void EXPECT_OPERAND_VM_STACK_EQ_WITH_CONSOLE_INPUT(
    const std::string& source,
    const std::string& consoleInput,
    const ISA::Type expectedType,
    const T expectedValue
) {
    try {
        std::stringstream input(consoleInput);
        std::streambuf* oldCin = std::cin.rdbuf(input.rdbuf());

        Assembler assembler;
        VM vm;

        const auto bytecode = assembler.assembleString(source);

        vm.run(&bytecode.value());

        std::cin.rdbuf(oldCin);

        const Value result = vm.popOperandStack();

        EXPECT_EQ(result.type, expectedType);
        auto resultVal = std::get<T>(result.toTyped());
        EXPECT_EQ(resultVal, expectedValue);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

inline void EXPECT_VM_ERROR_WITH_CONSOLE_INPUT(
    const std::string& source,
    const std::string& consoleInput
) {
    std::stringstream input(consoleInput);
    std::streambuf* oldCin = std::cin.rdbuf(input.rdbuf());

    Assembler assembler;
    VM vm;

    const auto bytecode = assembler.assembleString(source);

    EXPECT_THROW({
        vm.run(&bytecode.value());
    }, VMError);

    std::cin.rdbuf(oldCin);
}

inline void EXPECT_CONSOLE_OUTPUT(
    const std::string& source,
    const std::string& output
) {
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    Assembler assembler;
    VM vm;

    const auto bytecode = assembler.assembleString(source);

    vm.run(&bytecode.value());

    std::cout.rdbuf(old);
    EXPECT_EQ(buffer.str(), output);
}

#endif //SVM_VMTESTUTILS_H