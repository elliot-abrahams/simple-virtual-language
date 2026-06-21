#include "VM.h"

#include <ios>
#include <iostream>

#include "TypeConversions.h"
#include  "ArithmeticOps.h"

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

    while (running) {
        this->execute();
    }
}

void VM::setHP(const uint32_t hp) {
    this->HP = hp;
}

Value VM::peekOperandStack() const {
    return this->operandStack.peek();
}

void VM::execute() {
    // read byte at PC
    const uint8_t opcode = this->memoryManager.read8(MemoryAccessScope::CODE, this->PC++);

    switch (static_cast<ISA::Opcode>(opcode)) {
        case ISA::Opcode::NOP: break; // nop
        case ISA::Opcode::HALT: this->executeHalt(); break; // halt

        // -------------------------------------------------
        // STACK
        // -------------------------------------------------

        case ISA::Opcode::PUSH: this->executePush(); break; // push
        case ISA::Opcode::POP: this->executePop(); break; // pop
        case ISA::Opcode::DUP: this->executeDup(); break; // dup
        case ISA::Opcode::SWAP: this->executeSwap(); break; // swap

        // -------------------------------------------------
        // MEMORY
        // -------------------------------------------------

        case ISA::Opcode::LOAD: this->executeLoad(); break; // load
        case ISA::Opcode::LOADG: this->executeLoadG(); break; // loadG
        case ISA::Opcode::LOADL: this->executeLoadL(); break; // loadL
        case ISA::Opcode::STORE: this->executeStore(); break; // store
        case ISA::Opcode::STOREG: this->executeStoreG(); break; // storeG
        case ISA::Opcode::STOREL: this->executeStoreL(); break; // storeL
        case ISA::Opcode::ALLOC: break; // alloc
        case ISA::Opcode::FREE: break; // free

        // -------------------------------------------------
        // CONTROL
        // -------------------------------------------------

        case ISA::Opcode::CALL: break; // call
        case ISA::Opcode::RET: break; // ret
        case ISA::Opcode::JMP: break; // jmp
        case ISA::Opcode::JEZ: break; // jez
        case ISA::Opcode::JNZ: break; // jnz

        // -------------------------------------------------
        // ARITHMETIC
        // -------------------------------------------------

        case ISA::Opcode::ADD: this->executeAdd(); break; // add
        case ISA::Opcode::SUB: this->executeSub(); break; // sub
        case ISA::Opcode::MUL: this->executeMul(); break; // mul
        case ISA::Opcode::DIV: this->executeDiv(); break; // div
        case ISA::Opcode::MOD: this->executeMod(); break; // mod
        case ISA::Opcode::NOT: this->executeNot(); break; // not
        case ISA::Opcode::AND: this->executeAnd(); break; // and
        case ISA::Opcode::ORR: this->executeOrr(); break; // orr
        case ISA::Opcode::XOR: this->executeXor(); break; // xor
        case ISA::Opcode::SHL: this->executeShl(); break; // shl
        case ISA::Opcode::SHR: this->executeShr(); break; // shr
        case ISA::Opcode::SAR: this->executeSar(); break; // sar
        case ISA::Opcode::CEQ: break; // ceq
        case ISA::Opcode::CNE: break; // cne
        case ISA::Opcode::CLT: break; // clt
        case ISA::Opcode::CLE: break; // cle
        case ISA::Opcode::CGT: break; // cgt
        case ISA::Opcode::CGE: break; // cge

        // -------------------------------------------------
        // OTHER
        // -------------------------------------------------

        case ISA::Opcode::OUT: break; // out
        case ISA::Opcode::INN: break; // inn
        case ISA::Opcode::CONV: break; // conv
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
    VM::checkType("load", static_cast<uint8_t>(ISA::Type::PTR), static_cast<uint8_t>(address.type)); // ensure type of address is of type ptr

    const uint64_t value = this->memoryManager.read64(MemoryAccessScope::PTR, address.rawValue); // read value at address
    this->operandStack.push(type, value); // push value onto operand stack
}

void VM::executeLoadG() {
    const uint32_t address = this->fetchOperand(static_cast<uint8_t>(ISA::Type::PTR)); // read label operand
    const uint8_t valueDataType = this->memoryManager.read8(MemoryAccessScope::DATA, address - 1); // load data type at address

    /*
     * <data type of global at address>
     * STR -> push address onto operand stack
     * NOT STR -> push value at address onto operand stack
    */

    if (valueDataType != static_cast<uint8_t>(ISA::Type::STR)) {
        const uint64_t value = this->memoryManager.read64(MemoryAccessScope::PTR, address); // read value at address
        this->operandStack.push(valueDataType, value); // push value onto operand stack
    } else {
        // push address (pointer to the string) onto operand stack
        this->operandStack.push(valueDataType, address);
    }
}

void VM::executeLoadL() {
    const uint8_t type = this->fetchType(); // read type operand
    const uint64_t rawOffset = this->fetchOperand(static_cast<uint8_t>(ISA::Type::I32)); // read immediate operand
    const int32_t offset = TypeConversions::rawToI32(rawOffset);

    const uint32_t address = this->FP + ((static_cast<int32_t>(offset) - 1) * 8); // calculate address from given operand immediate

    const uint64_t local = this->memoryManager.read64(MemoryAccessScope::DATA, address); // get local value from memory

    this->operandStack.push(static_cast<uint8_t>(type), local); // push local value onto operand stack
}

void VM::executeStore() {
    const Value value = this->operandStack.pop(); // pop value to store from operand stack
    const Value address = this->operandStack.pop(); // pop address to store to from operand stack
    VM::checkType("store", static_cast<uint8_t>(ISA::Type::PTR), static_cast<uint8_t>(address.type)); // ensure type of address is of type ptr

    // store value in memory at address
    this->memoryManager.write(MemoryAccessScope::PTR, address.rawValue, &value);
}

void VM::executeStoreG() {
    const uint8_t address = this->fetchOperand(static_cast<uint8_t>(ISA::Type::PTR)); // read label operand
    const Value value = this->operandStack.pop(); // pop value from operand stack to store

    // ensure the value on the operand stack matches the type of the target global
    const uint8_t valueDataType = this->memoryManager.read8(MemoryAccessScope::DATA, address - 1); // read data type of target global
    this->checkType("storeG", valueDataType, static_cast<uint8_t>(value.type)); // ensure type of target global matches type of value from operand stack

    this->memoryManager.write(MemoryAccessScope::PTR, address, &value); // store val in memory at address
}

void VM::executeStoreL() {
    const uint64_t rawOffset = this->fetchOperand(static_cast<uint8_t>(ISA::Type::I32)); // read immediate operand
    const int32_t offset = TypeConversions::rawToI32(rawOffset);
    const Value value = this->operandStack.pop(); // pop value from operand stack to store

    const uint32_t address = this->FP + ((offset - 1) * 8); // calculate address from given operand immediate

    this->memoryManager.write64(MemoryAccessScope::CALL_STACK, address, value.rawValue); // store value from operand stack to memory
}

// -------------------------------------------------
// Memory
// -------------------------------------------------

void VM::executeAdd() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop();
    const Value value2 = this->operandStack.pop();

    // add those two values and push the result onto the operand stack
    this->operandStack.push(ArithmeticOps::add(value2, value1));
}

void VM::executeSub() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop();
    const Value value2 = this->operandStack.pop();

    // subtract those two values and push the result onto the operand stack
    this->operandStack.push(ArithmeticOps::sub(value2, value1));
}

void VM::executeMul() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop();
    const Value value2 = this->operandStack.pop();

    // multiply those two values and push the result onto the operand stack
    this->operandStack.push(ArithmeticOps::mul(value2, value1));
}

void VM::executeDiv() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop();
    const Value value2 = this->operandStack.pop();

    // multiply those two values and push the result onto the operand stack
    this->operandStack.push(ArithmeticOps::div(value2, value1));
}

void VM::executeMod() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop();
    const Value value2 = this->operandStack.pop();

    // compute modulo and push the result onto the operand stack
    this->operandStack.push(ArithmeticOps::mod(value2, value1));
}

void VM::executeNot() {
    const Value value = this->operandStack.pop(); // pop value from the operand stack

    // compute bitwise not and push the result onto the operand stack
    this->operandStack.push(ArithmeticOps::bitwiseNot(value));
}

void VM::executeAnd() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop();
    const Value value2 = this->operandStack.pop();

    // compute bitwise and, and push the result onto the operand stack
    this->operandStack.push(ArithmeticOps::bitwiseAnd(value2, value1));
}

void VM::executeOrr() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop();
    const Value value2 = this->operandStack.pop();

    // compute bitwise or, and push the result onto the operand stack
    this->operandStack.push(ArithmeticOps::bitwiseOr(value2, value1));
}

void VM::executeXor() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop();
    const Value value2 = this->operandStack.pop();

    // compute bitwise xor and push the result onto the operand stack
    this->operandStack.push(ArithmeticOps::bitwiseXor(value2, value1));
}

void VM::executeShl() {
    // pop two values off of the operand stack
    const Value numberOfShifts = this->operandStack.pop();
    const Value value = this->operandStack.pop();

    // compute logical shift left and push the result onto the operand stack
    this->operandStack.push(ArithmeticOps::shl(value, numberOfShifts));
}

void VM::executeShr() {
    // pop two values off of the operand stack
    const Value numberOfShifts = this->operandStack.pop();
    const Value value = this->operandStack.pop();

    // compute logical shift right and push the result onto the operand stack
    this->operandStack.push(ArithmeticOps::shr(false, value, numberOfShifts));
}

void VM::executeSar() {
    // pop two values off of the operand stack
    const Value numberOfShifts = this->operandStack.pop();
    const Value value = this->operandStack.pop();

    // compute arithmetic shift right and push the result onto the operand stack
    this->operandStack.push(ArithmeticOps::shr(true, value, numberOfShifts));
}


uint8_t VM::fetchType() {
    return this->memoryManager.read8(MemoryAccessScope::CODE, this->PC++);
}

uint64_t VM::fetchOperand(const uint8_t type) {
    uint64_t result = 0;
    switch (static_cast<ISA::Type>(type)) {
        case ISA::Type::I32: // i32
        case ISA::Type::UI32: // ui32
        case ISA::Type::F32: // f32
        case ISA::Type::PTR: // ptr
        {
            result = static_cast<uint64_t>(this->memoryManager.read32(MemoryAccessScope::CODE, this->PC));
            this->PC += 4;
            break;
        }
        case ISA::Type::I64: // i64
        case ISA::Type::UI64: // ui64
        case ISA::Type::F64: // f64
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

    std::cerr << "\nOperand Stack (bottom -> top):\n";
    for (int i = 0; i < stack->size(); i++) {
        std::cerr << "0x" << std::hex << i << ": ";

        std::visit([](auto&& val){
            std::cerr << std::dec << val << "\n";
        }, stack->at(i).toTyped());
    }

    std::cerr << std::dec << std::endl;
}

void VM::checkType(const std::string &instructionMnemonic, const uint8_t expectedType, const uint8_t actualType) {
    if (expectedType != actualType) {
        throw VMError(
            std::string("Error: type mismatch") +
            "\nInstruction: " + instructionMnemonic +
            "\nExpected: " + TypeConversions::typeToString(expectedType) +
            "\nActual: " + TypeConversions::typeToString(actualType)
        );
    }
}
