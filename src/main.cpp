#include "Driver.h"
#include "assembler/Assembler.h"
#include "vm/VM.h"

int main(int argc, char* argv[]) {

    Assembler* assembler = new Assembler();
    auto bytecode = assembler->assemble("../examples/vm-test.svma");
    if (!bytecode.has_value()) {
        return 500;
    }
    delete assembler;

    VM* vm = new VM();
    vm->run(&bytecode.value());
    delete vm;
    return 0;

    return Driver::runCLI(argc, argv);
}
