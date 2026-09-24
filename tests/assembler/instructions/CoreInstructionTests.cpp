#include <gtest/gtest.h>
#include "../AssemblerTestsUtils.h"

using namespace assemblerTests;

TEST(CORE, ENCODING) {
    ASSERT_BYTECODE_EQ(
        R"(
            halt
            nop
            halt
        )",
        std::vector<uint8_t>{
            0x16, // header -> data
            0x00,
            0x00,
            0x00,
            0x16, // header -> sources
            0x00,
            0x00,
            0x00,
            0x16, // header -> functions
            0x00,
            0x00,
            0x00,
            0x16, // header -> line_table
            0x00,
            0x00,
            0x00,
            0x16, // header -> end
            0x00,
            0x00,
            0x00,
            0x01, // halt
            0x00, // nop
            0x01  // halt
        }
    );
}