#include "assembler/Assembler.h"
#include "vm/VM.h"

int main() {

    Assembler* assembler = new Assembler();
    VM* vm = new VM();

    // assemble SVMA
    auto byteCode = assembler->assemble("../examples/vm-test.svma");

    if (!byteCode.has_value()) {
        return 500;
    }

    delete assembler;

    // run SVMA
    vm->run(byteCode.value());

    return 0;
}
