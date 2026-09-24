#ifndef SV_ASSEMBLERTESTSUTILS_H
#define SV_ASSEMBLERTESTSUTILS_H
#include <cstdint>
#include <iostream>
#include <vector>


#include "../../src/assembler/Assembler.h"
#include "../../src/include/Error.h"
#include "gtest/gtest.h"

namespace assemblerTests {

    inline void ASSERT_BYTECODE_EQ(
        const std::string& source,
        const std::vector<uint8_t>& expectedBytecode
    ) {
        const auto assembler = new assembler::Assembler();
        std::vector<uint8_t> bytecode;
        try {
            bytecode = assembler->assembleString(source);
        } catch (const AssemblerError& e) {
            std::cerr << e.generateMessage() << std::endl;
            FAIL();
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
            FAIL();
        }
        ASSERT_EQ(bytecode, expectedBytecode);
    }

    inline void ASSERT_THROWS_ASSEMBLER_ERROR(
        const std::string& source
    ) {
        const auto assembler = new assembler::Assembler();
        ASSERT_THROW(assembler->assembleString(source), AssemblerError);
    }

    inline void ASSERT_NO_ASSEMBLER_ERROR(
        const std::string& source
    ) {
        const auto assembler = new assembler::Assembler();
        ASSERT_NO_THROW(assembler->assembleString(source));
    }
}


#endif //SV_ASSEMBLERTESTSUTILS_H