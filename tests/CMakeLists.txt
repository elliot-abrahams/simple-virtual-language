cmake_minimum_required(VERSION 3.20)
project(svm)

set(This vm_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Sources
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

add_executable(${This} ${Sources})

target_link_libraries(${This} PRIVATE
    gtest_main
    svm_lib
)

include(GoogleTest)
gtest_discover_tests(${This})