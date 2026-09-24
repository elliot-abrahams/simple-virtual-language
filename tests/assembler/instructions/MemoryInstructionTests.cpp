#include <gtest/gtest.h>
#include "../AssemblerTestsUtils.h"

using namespace assemblerTests;

TEST(MEMORY, ENCODING) {
    ASSERT_BYTECODE_EQ(
    R"(
            load i32
            load ui32
            load i64
            load ui64
            load f32
            load f64
            load ptr

            loadB

            loadG $x

            loadL i32 #5
            loadL ui32 #5
            loadL i64 #5
            loadL ui64 #5
            loadL f32 #5
            loadL f64 #5
            loadL ptr #5

            store
            storeB
            storeG $x
            storeL #5
            addrL #5
            alloc
            free

        .data
        $x: i32 5
        )",
        std::vector<uint8_t>{
            0x64, // header -> data
            0x00,
            0x00,
            0x00,
            0x69, // header -> sources
            0x00,
            0x00,
            0x00,
            0x69, // header -> functions
            0x00,
            0x00,
            0x00,
            0x69, // header -> line_table
            0x00,
            0x00,
            0x00,
            0x69, // header -> end
            0x00,
            0x00,
            0x00,
            0x08, // load
            0x00, // i32
            0x08, // load
            0x01, // ui32
            0x08, // load
            0x02, // i64
            0x08, // load
            0x03, // ui64
            0x08, // load
            0x04, // f32
            0x08, // load
            0x05, // f64
            0x08, // load
            0x06, // ptr
            0x09, // loadB
            0x0a, // loadG
            0x52, // $x
            0x00,
            0x00,
            0x00,
            0x0b, // loadL
            0x00, // i32
            0x05, // #5
            0x00,
            0x00,
            0x00,
            0x0b, // loadL
            0x01, // ui32
            0x05, // #5
            0x00,
            0x00,
            0x00,
            0x0b, // loadL
            0x02, // i64
            0x05, // #5
            0x00,
            0x00,
            0x00,
            0x0b, // loadL
            0x03, // i64
            0x05, // #5
            0x00,
            0x00,
            0x00,
            0x0b, // loadL
            0x04, // f32
            0x05, // #5
            0x00,
            0x00,
            0x00,
            0x0b, // loadL
            0x05, // f64
            0x05, // #5
            0x00,
            0x00,
            0x00,
            0x0b, // loadL
            0x06, // ptr
            0x05, // #5
            0x00,
            0x00,
            0x00,
            0x0c, // store
            0x0d, // storeB
            0x0e, // storeG
            0x52, // $x
            0x00,
            0x00,
            0x00,
            0x0f, // storeL
            0x05, // #5
            0x00,
            0x00,
            0x00,
            0x10, // addrL
            0x05, // #5
            0x00,
            0x00,
            0x00,
            0x11, // alloc
            0x12, // free
            0x00, // i32
            0x05, // 5
            0x00,
            0x00,
            0x00
        }
    );
}

TEST(MEMORY, INVALID_LOAD_TWO_OPERANDS) {
    ASSERT_THROWS_ASSEMBLER_ERROR("load i32 #5");
}

TEST(MEMORY, INVALID_LOAD_IMMEDIATE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("load #5");
}

TEST(MEMORY, INVALID_LOAD_LABEL_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$label: load $label");
}

TEST(MEMORY, INVALID_LOAD_NATIVE_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("load exit");
}

TEST(MEMORY, INVALID_LOAD_ERROR_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("load array_index_out_of_range");
}

TEST(MEMORY, INVALID_LOADB_TYPE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("loadB i32");
}

TEST(MEMORY, INVALID_LOADB_IMMEDIATE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("loadB #5");
}

TEST(MEMORY, INVALID_LOADB_LABEL_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$label: loadB $label");
}

TEST(MEMORY, INVALID_LOADB_NATIVE_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("loadB exit");
}

TEST(MEMORY, INVALID_LOADB_ERROR_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("loadB array_index_out_of_range");
}

TEST(MEMORY, INVALID_LOADG_TWO_OPERANDS) {
    ASSERT_THROWS_ASSEMBLER_ERROR("loadG $x #5 .data $x: i32 5");
}

TEST(MEMORY, INVALID_LOADG_TYPE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("loadG i32");
}

TEST(MEMORY, INVALID_LOADG_IMMEDIATE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("loadG #5");
}

TEST(MEMORY, INVALID_LOADG_NATIVE_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("loadG exit");
}

TEST(MEMORY, INVALID_LOADG_ERROR_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("loadG array_index_out_of_range");
}

TEST(MEMORY, INVALID_LOADL_THREE_OPERANDS) {
    ASSERT_THROWS_ASSEMBLER_ERROR("loadL i32 #5 #5");
}

TEST(MEMORY, INVALID_LOADL_TYPE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("loadL i32 i32");
}

TEST(MEMORY, INVALID_LOADL_IMMEDIATE_ZERO) {
    ASSERT_THROWS_ASSEMBLER_ERROR("loadL #0");
}

TEST(MEMORY, INVALID_LOADL_IMMEDIATE_DECIMAL) {
    ASSERT_THROWS_ASSEMBLER_ERROR("loadL #1.5");
}

TEST(MEMORY, INVALID_LOADL_LABEL_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$label: loadL i32 $label");
}

TEST(MEMORY, INVALID_LOADL_NATIVE_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("loadL i32 exit");
}

TEST(MEMORY, INVALID_LOADL_ERROR_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("loadL i32 array_index_out_of_range");
}

TEST(MEMORY, INVALID_STORE_TYPE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("store i32");
}

TEST(MEMORY, INVALID_STORE_IMMEDIATE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("store #5");
}

TEST(MEMORY, INVALID_STORE_LABEL_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$label: store $label");
}

TEST(MEMORY, INVALID_STORE_NATIVE_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("store exit");
}

TEST(MEMORY, INVALID_STORE_ERROR_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("store array_index_out_of_range");
}

TEST(MEMORY, INVALID_STOREB_TYPE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("storeB i32");
}

TEST(MEMORY, INVALID_STOREB_IMMEDIATE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("storeB #5");
}

TEST(MEMORY, INVALID_STOREB_LABEL_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$label: storeB $label");
}

TEST(MEMORY, INVALID_STOREB_NATIVE_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("storeB exit");
}

TEST(MEMORY, INVALID_STOREB_ERROR_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("storeB array_index_out_of_range");
}

TEST(MEMORY, INVALID_STOREG_TWO_OPERANDS) {
    ASSERT_THROWS_ASSEMBLER_ERROR("storeG $x #5 .data $x: i32 5");
}

TEST(MEMORY, INVALID_STOREG_TYPE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("storeG i32");
}

TEST(MEMORY, INVALID_STOREG_IMMEDIATE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("storeG #5");
}

TEST(MEMORY, INVALID_STOREG_NATIVE_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("storeG exit");
}

TEST(MEMORY, INVALID_STOREG_ERROR_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("storeG array_index_out_of_range");
}

TEST(MEMORY, INVALID_STOREL_TWO_OPERANDS) {
    ASSERT_THROWS_ASSEMBLER_ERROR("storeL #5 #5");
}

TEST(MEMORY, INVALID_STOREL_IMMEDIATE_ZERO) {
    ASSERT_THROWS_ASSEMBLER_ERROR("storeL #0");
}

TEST(MEMORY, INVALID_STOREL_IMMEDIATE_DECIMAL) {
    ASSERT_THROWS_ASSEMBLER_ERROR("storeL #1.5");
}

TEST(MEMORY, INVALID_STOREL_TYPE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("storeL i32");
}

TEST(MEMORY, INVALID_STOREL_LABEL_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$label: storeL $label");
}

TEST(MEMORY, INVALID_STOREL_NATIVE_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("storeL exit");
}

TEST(MEMORY, INVALID_STOREL_ERROR_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("storeL array_index_out_of_range");
}

TEST(MEMORY, INVALID_ADDRL_TWO_OPERANDS) {
    ASSERT_THROWS_ASSEMBLER_ERROR("addrL #5 #5");
}

TEST(MEMORY, INVALID_ADDRL_IMMEDIATE_ZERO) {
    ASSERT_THROWS_ASSEMBLER_ERROR("addrL #0");
}

TEST(MEMORY, INVALID_ADDRL_IMMEDIATE_DECIMAL) {
    ASSERT_THROWS_ASSEMBLER_ERROR("addrL #1.5");
}

TEST(MEMORY, INVALID_ADDRL_TYPE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("addrL i32");
}

TEST(MEMORY, INVALID_ADDRL_LABEL_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$label: addrL $label");
}

TEST(MEMORY, INVALID_ADDRL_NATIVE_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("addrL exit");
}

TEST(MEMORY, INVALID_ADDRL_ERROR_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("addrL array_index_out_of_range");
}

TEST(MEMORY, INVALID_ALLOC_TYPE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("alloc i32");
}

TEST(MEMORY, INVALID_ALLOC_IMMEDIATE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("alloc #5");
}

TEST(MEMORY, INVALID_ALLOC_LABEL_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$label: alloc $label");
}

TEST(MEMORY, INVALID_ALLOC_NATIVE_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("alloc exit");
}

TEST(MEMORY, INVALID_ALLOC_ERROR_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("alloc array_index_out_of_range");
}

TEST(MEMORY, INVALID_FREE_TYPE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("free i32");
}

TEST(MEMORY, INVALID_FREE_IMMEDIATE) {
    ASSERT_THROWS_ASSEMBLER_ERROR("free #5");
}

TEST(MEMORY, INVALID_FREE_LABEL_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("$label: free $label");
}

TEST(MEMORY, INVALID_FREE_NATIVE_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("free exit");
}

TEST(MEMORY, INVALID_FREE_ERROR_REF) {
    ASSERT_THROWS_ASSEMBLER_ERROR("free array_index_out_of_range");
}

