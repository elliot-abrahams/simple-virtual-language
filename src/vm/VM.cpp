#include "VM.h"

#include <ios>
#include <iostream>

VM::VM() :
    PC(0),
    HP(0),
    FP(MAX_MEMORY_ADDRESS),
    SP(MAX_MEMORY_ADDRESS),
    memoryManager(MemoryManager(&HP, &SP)),
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
    const uint8_t opcode = this->memoryManager.read8(MemoryAccessScope::CODE, this->PC++);

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

        case 0x06: this->executeLoad(); break; // load
        case 0x07: this->executeLoadG(); break; // loadG
        case 0x08: this->executeLoadL(); break; // loadL
        case 0x09: this->executeStore(); break; // store
        case 0x0a: this->executeStoreG(); break; // storeG
        case 0x0b: this->executeStoreL(); break; // storeL
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

// -------------------------------------------------
// STACK
// -------------------------------------------------

void VM::executePush() {
    const uint8_t type = this->fetchType(); // read type operand
    const uint64_t value = this->fetchOperand(type); // get value from bytecode
    this->operandStack.push(type, value); // push value onto the operand stack
}

void VM::executePop() {
    this->operandStack.pop(); // remove top of stack
}

void VM::executeDup() {
    const Value val = this->operandStack.peek(); // get top value of operand stack
    this->operandStack.push(static_cast<uint8_t>(val.type), val.rawValue); // push that value onto the operand stack
}

void VM::executeSwap() {
    // pop top two values from operand stack
    const Value val1 = this->operandStack.pop();
    const Value val2 = this->operandStack.pop();

    // push the two values onto the operand stack but swapped
    this->operandStack.push(static_cast<uint8_t>(val1.type), val1.rawValue);
    this->operandStack.push(static_cast<uint8_t>(val2.type), val2.rawValue);
}

// -------------------------------------------------
// Memory
// -------------------------------------------------

void VM::executeLoad() {
    const uint8_t type = this->fetchType(); // read type operand
    const Value address = this->operandStack.pop(); // pop address from operand stack
    VM::checkType("load", static_cast<uint8_t>(Type::PTR), static_cast<uint8_t>(address.type)); // ensure type of address is of type ptr

    const uint64_t value = this->memoryManager.read64(MemoryAccessScope::PTR, address.rawValue); // read value at address
    this->operandStack.push(type, value); // push value onto operand stack
}

void VM::executeLoadG() {
    const uint32_t address = this->fetchOperand(static_cast<uint8_t>(Type::PTR)); // read label operand
    const uint8_t valueDataType = this->memoryManager.read8(MemoryAccessScope::DATA, address - 1); // load data type at address

    /*
     * <data type of global at address>
     * STR -> push address onto operand stack
     * NOT STR -> push value at address onto operand stack
    */

    if (valueDataType != static_cast<uint8_t>(Type::STR)) {
        const uint64_t value = this->memoryManager.read64(MemoryAccessScope::PTR, address); // read value at address
        this->operandStack.push(valueDataType, value); // push value onto operand stack
    } else {
        // push address (pointer to the string) onto operand stack
        this->operandStack.push(valueDataType, address);
    }
}

void VM::executeLoadL() {
    const uint8_t type = this->fetchType(); // read type operand
    const uint64_t rawOffset = this->fetchOperand(static_cast<uint8_t>(Type::I32)); // read immediate operand
    const int32_t offset = VM::interpretI32(rawOffset);

    const uint32_t address = this->FP + ((static_cast<int32_t>(offset) - 1) * 8); // calculate address from given operand immediate

    const uint64_t local = this->memoryManager.read64(MemoryAccessScope::DATA, address); // get local value from memory

    this->operandStack.push(static_cast<uint8_t>(type), local); // push local value onto operand stack
}

void VM::executeStore() {
    const Value value = this->operandStack.pop(); // pop value to store from operand stack
    const Value address = this->operandStack.pop(); // pop address to store to from operand stack
    VM::checkType("store", static_cast<uint8_t>(Type::PTR), static_cast<uint8_t>(address.type)); // ensure type of address is of type ptr

    // store value in memory at address
    const Value* pValue = &value;
    this->memoryManager.write(MemoryAccessScope::PTR, address.rawValue, pValue);
}

void VM::executeStoreG() {
    const uint8_t address = this->fetchOperand(static_cast<uint8_t>(Type::PTR)); // read label operand
    const Value value = this->operandStack.pop(); // pop value from operand stack to store

    // ensure the value on the operand stack matches the type of the target global
    const uint8_t valueDataType = this->memoryManager.read8(MemoryAccessScope::DATA, address - 1); // read data type of target global
    this->checkType("storeG", valueDataType, static_cast<uint8_t>(value.type));

    this->memoryManager.write(MemoryAccessScope::PTR, address, &value); // store val in memory at address
}

void VM::executeStoreL() {
    const uint64_t rawOffset = this->fetchOperand(static_cast<uint8_t>(Type::I32)); // read immediate operand
    const int32_t offset = VM::interpretI32(rawOffset);
    const Value value = this->operandStack.pop(); // pop value from operand stack to store

    const uint32_t address = this->FP + ((offset - 1) * 8); // calculate address from given operand immediate

    this->memoryManager.write64(MemoryAccessScope::CALL_STACK, address, value.rawValue);
}

uint8_t VM::fetchType() {
    return this->memoryManager.read8(MemoryAccessScope::CODE, this->PC++);
}

uint64_t VM::fetchOperand(const uint8_t type) {
    uint64_t result = 0;
    switch (type) {
        case 0x00: // i32
        case 0x01: // ui32
        case 0x04: // f32
        case 0x06: // ptr
        {
            result = static_cast<uint64_t>(this->memoryManager.read32(MemoryAccessScope::CODE, this->PC));
            this->PC += 4;
            break;
        }
        case 0x02: // i64
        case 0x03: // ui64
        case 0x05: // f64
        case 0x07: // char
        {
            result = this->memoryManager.read64(MemoryAccessScope::CODE, this->PC);
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

void VM::checkType(const std::string instruction, const uint8_t expectedType, const uint8_t actualType) {
    if (expectedType != actualType) {
        throw VMError(
            std::string("Error: type mismatch") +
            "\nInstruction: " + instruction +
            "\nExpected: " + typeToString(expectedType) +
            "\nActual: " + typeToString(actualType)
        );
    }
}

std::string VM::typeToString(const uint8_t type) {
    switch (type) {
        case static_cast<uint8_t>(Type::I32): return "i32";
        case static_cast<uint8_t>(Type::UI32): return "ui64";
        case static_cast<uint8_t>(Type::I64): return "i64";
        case static_cast<uint8_t>(Type::UI64): return "ui64";
        case static_cast<uint8_t>(Type::F32): return "f32";
        case static_cast<uint8_t>(Type::F64): return "f64";
        case static_cast<uint8_t>(Type::PTR): return "ptr";
        case static_cast<uint8_t>(Type::CHAR): return "char";
        case static_cast<uint8_t>(Type::STR): return "str";
        default: return "Unknown";
    }
}

int32_t VM::interpretI32(const uint64_t rawValue) {
    return static_cast<int32_t>(static_cast<uint32_t>(rawValue));
}
