#include "Driver.h"
#include "assembler/Assembler.h"
#include "vm/VM.h"
#include <windows.h>

#include "compiler/Compiler.h"

int main(int argc, char* argv[]) {

    compiler::Compiler* compiler = new compiler::Compiler();

    compiler->compile("../examples/test.sv");


    /*
    // set output to UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    Assembler* assembler = new Assembler();
    auto bytecode = assembler->assemble("../examples/vm-test.svma");
    if (!bytecode.has_value()) {
        return 500;
    }
    delete assembler;

    VM* vm = new VM();
    try {
        vm->run(&bytecode.value());
    } catch (VMError& e) {
        vm->handleVMError(e);
    }
    vm->dumpState();
    delete vm;
    return 0;

    return Driver::runCLI(argc, argv);

    */
}
