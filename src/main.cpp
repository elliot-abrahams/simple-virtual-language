#include <iostream>

#include "Driver.h"
#include "assembler/Assembler.h"
#include "vm/VM.h"
#include <windows.h>

#include "compiler/Compiler.h"

int main(int argc, char* argv[]) {
    // set output to UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    return Driver::runCLI(argc, argv);
}
