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
        const Value result = vm.peekOperandStack();

        EXPECT_EQ(result.type, expectedType);
        auto resultVal = std::get<T>(result.toTyped());
        EXPECT_EQ(resultVal, expectedValue);
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

#endif //SVM_VMTESTUTILS_H