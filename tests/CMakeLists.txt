cmake_minimum_required(VERSION 3.20)
project(svm)

set(This vm_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Sources
        vm/runtime/arithmetic/cgeTests.cpp
        vm/runtime/arithmetic/cgtTests.cpp
        vm/runtime/arithmetic/cleTests.cpp
        vm/runtime/arithmetic/cltTests.cpp
        vm/runtime/arithmetic/cneTests.cpp
        vm/runtime/arithmetic/ceqTests.cpp
        vm/runtime/arithmetic/sarTests.cpp
        vm/runtime/arithmetic/shrTests.cpp
        vm/runtime/arithmetic/shlTests.cpp
        vm/runtime/arithmetic/xorTests.cpp
        vm/runtime/arithmetic/orrTests.cpp
        vm/runtime/arithmetic/andTests.cpp
        vm/runtime/arithmetic/notTests.cpp
        vm/runtime/arithmetic/modTests.cpp
        vm/runtime/arithmetic/divTests.cpp
        vm/runtime/arithmetic/mulTests.cpp
        vm/runtime/arithmetic/subTests.cpp
        vm/runtime/arithmetic/addTests.cpp
        vm/vmTestUtils.h
        vm/vm_Tests.cpp
)

add_executable(${This} ${Sources})

target_link_libraries(${This} PRIVATE
    gtest_main
    svm_lib
)

include(GoogleTest)
gtest_discover_tests(${This})