#include "VM.h"

#include <ios>
#include <iostream>

VM::VM() :
    PC(0),
    HP(0),
    FP(MAX_MEMORY_ADDRESS),
    SP(MAX_MEMORY_ADDRESS),
    memoryManager(MemoryManager()),
    operandStack(OperandStack()),
    running(true) {}

void VM::run(const std::vector<uint8_t>* bytecode) {
    // load bytecode into memory
    this->memoryManager.loadBytecodeIntoMemory(bytecode);
    // set HP
    this->HP = bytecode->size() - BYTECODE_HEADER_SIZE;

    try {
        while (running) {
            this->execute();
        }
    } catch (VMError& e) {
        handleVMError(e);
    }
}

void VM::setHP(const uint32_t hp) {
    this->HP = hp;
}

void VM::execute() {
    // read byte at PC
    uint8_t opcode = this->memoryManager.read8(PC++);

    switch (opcode) {
        case 0x00: { // nop
            break;
        }

        case 0x01: { // halt
            this->executeHalt();
            break;
        }

        // -------------------------------------------------
        // STACK
        // -------------------------------------------------

        case 0x02: { // push
            break;
        }

        case 0x03: { // pop
            break;
        }

        case 0x04: { // dup
            break;
        }

        case 0x05: { // swap
            break;
        }

        // -------------------------------------------------
        // MEMORY
        // -------------------------------------------------

        case 0x06: { // load
            break;
        }

        case 0x07: { // loadG
            break;
        }

        case 0x08: { // loadL
            break;
        }

        case 0x09: { // store
            break;
        }

        case 0x0a: { // storeG
            break;
        }

        case 0x0b: { // storeL
            break;
        }

        case 0x0c: { // alloc
            break;
        }

        case 0x0d: { // free
            break;
        }

        // -------------------------------------------------
        // CONTROL
        // -------------------------------------------------

        case 0x0e: { // call
            break;
        }

        case 0x0f: { // ret
            break;
        }

        case 0x10: { // jmp
            break;
        }

        case 0x11: { // jez
            break;
        }

        case 0x12: { // jnz
            break;
        }

        // -------------------------------------------------
        // ARITHMETIC
        // -------------------------------------------------

        case 0x13: { // add
            break;
        }

        case 0x14: { // sub
            break;
        }

        case 0x15: { // mul
            break;
        }

        case 0x16: { // div
            break;
        }

        case 0x17: { // mod
            break;
        }

        case 0x18: { // not
            break;
        }

        case 0x19: { // notB
            break;
        }

        case 0x1a: { // and
            break;
        }

        case 0x1b: { // orr
            break;
        }

        case 0x1c: { // xor
            break;
        }

        case 0x1d: { // shl
            break;
        }

        case 0x1e: { // shr
            break;
        }

        case 0x1f: { // ceq
            break;
        }

        case 0x20: { // cne
            break;
        }

        case 0x21: { // clt
            break;
        }

        case 0x22: { // cle
            break;
        }

        case 0x23: { // cgt
            break;
        }

        case 0x24: { // cge
            break;
        }

        // -------------------------------------------------
        // OTHER
        // -------------------------------------------------

        case 0x25: { // out
            break;
        }

        case 0x26: { // inn
            break;
        }

        case 0x27: { // conv
            break;
        }
    }
}


void VM::executeHalt() {
    this->running = false;
}

void VM::handleVMError(const VMError& e) const {
    std::cerr << "\n=== RUNTIME ERROR ===\n";
    std::cerr << e.what() << "\n\n";

    this->dumpState();

    std::cerr << "=====================\n";
}

void VM::dumpState() const {
    const auto& stack = this->operandStack.getStack();

    std::cerr << "--- VM STATE ---\n";

    std::cerr << "PC: 0x" << std::hex << PC << "\n";
    std::cerr << "HP: 0x" << std::hex << HP << "\n";
    std::cerr << "FP: 0x" << std::hex << FP << "\n";
    std::cerr << "SP: 0x" << std::hex << SP << "\n";

    std::cerr << "\nOperand Stack (top -> bottom):\n";
    for (int i = 0; i < stack->size(); i++) {
        std::cerr << "0x" << std::hex << i << ": ";

        std::visit([](auto&& val){
            std::cerr << std::dec << val << "\n";
        }, stack->at(i).toTyped());
    }

    std::cerr << std::dec << std::endl;
}
