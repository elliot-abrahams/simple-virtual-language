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
        this->dumpState();
    } catch (VMError& e) {
        handleVMError(e);
    }
}

void VM::setHP(const uint32_t hp) {
    this->HP = hp;
}

void VM::execute() {
    // read byte at PC
    const uint8_t opcode = this->memoryManager.read8(MemoryRegion::CODE, this->PC++);

    switch (opcode) {
        case 0x00: break; // nop
        case 0x01: this->executeHalt(); break; // halt

        // -------------------------------------------------
        // STACK
        // -------------------------------------------------

        case 0x02: this->executePush(); break; // push
        case 0x03: this->executePop(); break; // pop
        case 0x04: this->executeDup(); break; // dup
        case 0x05: this->executeSwap(); break; // swap

        // -------------------------------------------------
        // MEMORY
        // -------------------------------------------------

        case 0x06: break; // load
        case 0x07: break; // loadG
        case 0x08: break; // loadL
        case 0x09: break; // store
        case 0x0a: break; // storeG
        case 0x0b: break; // storeL
        case 0x0c: break; // alloc
        case 0x0d: break; // free

        // -------------------------------------------------
        // CONTROL
        // -------------------------------------------------

        case 0x0e: break; // call
        case 0x0f: break; // ret
        case 0x10: break; // jmp
        case 0x11: break; // jez
        case 0x12: break; // jnz

        // -------------------------------------------------
        // ARITHMETIC
        // -------------------------------------------------

        case 0x13: break; // add
        case 0x14: break; // sub
        case 0x15: break; // mul
        case 0x16: break; // div
        case 0x17: break; // mod
        case 0x18: break; // not
        case 0x19: break; // notB
        case 0x1a: break; // and
        case 0x1b: break; // orr
        case 0x1c: break; // xor
        case 0x1d: break; // shl
        case 0x1e: break; // shr
        case 0x1f: break; // ceq
        case 0x20: break; // cne
        case 0x21: break; // clt
        case 0x22: break; // cle
        case 0x23: break; // cgt
        case 0x24: break; // cge

        // -------------------------------------------------
        // OTHER
        // -------------------------------------------------

        case 0x25: break; // out
        case 0x26: break; // inn
        case 0x27: break; // conv
    }
}

void VM::executeHalt() {
    this->running = false;
}

void VM::executePush() {
    const uint8_t type = this->fetchType();
    const uint64_t value = this->fetchOperand(type);
    operandStack.push(type, value);
}

void VM::executePop() {
    // remove top of stack
    operandStack.pop();
}

void VM::executeDup() {
    const Value val = operandStack.peek();
    operandStack.push(static_cast<uint8_t>(val.type), val.rawValue);
}

void VM::executeSwap() {
    const Value val1 = operandStack.pop();
    const Value val2 = operandStack.pop();

    operandStack.push(static_cast<uint8_t>(val1.type), val1.rawValue);
    operandStack.push(static_cast<uint8_t>(val2.type), val2.rawValue);
}

uint8_t VM::fetchType() {
    return this->memoryManager.read8(MemoryRegion::CODE, this->PC++);
}

uint64_t VM::fetchOperand(const uint8_t type) {
    uint64_t result = 0;
    switch (type) {
        case 0x00: // i32
        case 0x01: // ui32
        case 0x04: // f32
        case 0x06: // ptr
        {
            result = static_cast<uint64_t>(this->memoryManager.read32(MemoryRegion::CODE, this->PC));
            this->PC += 4;
            break;
        }
        case 0x02: // i64
        case 0x03: // ui64
        case 0x05: // f64
        case 0x07: // char
        {
            result = this->memoryManager.read64(MemoryRegion::CODE, this->PC);
            this->PC += 8;
            break;
        }
    }
    return result;
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

    std::cerr << "PC: 0x" << std::hex << this->PC << "\n";
    std::cerr << "HP: 0x" << std::hex << this->HP << "\n";
    std::cerr << "FP: 0x" << std::hex << this->FP << "\n";
    std::cerr << "SP: 0x" << std::hex << this->SP << "\n";

    std::cerr << "\nOperand Stack (top -> bottom):\n";
    for (int i = 0; i < stack->size(); i++) {
        std::cerr << "0x" << std::hex << i << ": ";

        std::visit([](auto&& val){
            std::cerr << std::dec << val << "\n";
        }, stack->at(i).toTyped());
    }

    std::cerr << std::dec << std::endl;
}
