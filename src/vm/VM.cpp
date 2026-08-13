#include "VM.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <ios>
#include <iostream>

#include "TypeConversions.h"
#include "ArithmeticOps.h"
#include "NativeFunctionManager.h"
#include "VMError.h"
#include "memory/MemoryManager.h"

VM::VM() :
    PC(0),
    HB(0),
    HP(0),
    FP(MAX_MEMORY_ADDRESS),
    SP(MAX_MEMORY_ADDRESS),
    memoryManager(MemoryManager(&HB, &HP, &SP)),
    callStackManager(&memoryManager),
    heapManager(&memoryManager),
    operandStack(OperandStack()),
    running(true),
    exitCode(0) {}

void VM::run(const std::vector<uint8_t>* bytecode) {
    // load bytecode into memory
    this->memoryManager.loadBytecodeIntoMemory(bytecode);
    // set HB
    this->HB = bytecode->size() - BYTECODE_HEADER_SIZE;
    this->HP = this->HB;

    this->heapManager.initialiseHeap(&this->HP);

    while (running) {
        this->execute();
        if (this->exitCode != 0) return;
    }
}

Value VM::popOperandStack() {
    return this->operandStack.pop();
}

OperandStack* VM::getOperandStack() {
    return &this->operandStack;
}

void VM::setExitStatus(const int status) {
    this->exitCode = status;
}

int VM::getExitStatus() const {
    return this->exitCode;
}

std::string VM::readStringFromMemory(const uint32_t address) const {
    // read number of bytes of string
    const uint32_t size = this->memoryManager.read32(MemoryAccessScope::PTR, address);

    std::string string;
    string.resize(size);

    for (int i = 0; i < size; i++) {
        string[i] = static_cast<char>(this->memoryManager.read8(MemoryAccessScope::PTR, address + 4 + i));
    }
    return string;
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
        case ISA::Opcode::LOADB: this->executeLoadB(); break; // loadB
        case ISA::Opcode::LOADG: this->executeLoadG(); break; // loadG
        case ISA::Opcode::LOADL: this->executeLoadL(); break; // loadL
        case ISA::Opcode::STORE: this->executeStore(); break; // store
        case ISA::Opcode::STOREB: this->executeStoreB(); break; // storeB
        case ISA::Opcode::STOREG: this->executeStoreG(); break; // storeG
        case ISA::Opcode::STOREL: this->executeStoreL(); break; // storeL
        case ISA::Opcode::ALLOC: this->executeAlloc(); break; // alloc
        case ISA::Opcode::FREE: this->executeFree(); break; // free

        // -------------------------------------------------
        // CONTROL
        // -------------------------------------------------

        case ISA::Opcode::CALL: this->executeCall(); break; // call
        case ISA::Opcode::NATIVE: this->executeNative(); break; // native
        case ISA::Opcode::RET: this->executeRet(); break; // ret
        case ISA::Opcode::JMP: this->executeJmp(); break; // jmp
        case ISA::Opcode::JEZ: this->executeJez(); break; // jez
        case ISA::Opcode::JNZ: this->executeJnz(); break; // jnz

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
        case ISA::Opcode::CEQ: this->executeCeq(); break; // ceq
        case ISA::Opcode::CNE: this->executeCne(); break; // cne
        case ISA::Opcode::CLT: this->executeClt(); break; // clt
        case ISA::Opcode::CLE: this->executeCle(); break; // cle
        case ISA::Opcode::CGT: this->executeCgt(); break; // cgt
        case ISA::Opcode::CGE: this->executeCge(); break; // cge

        // -------------------------------------------------
        // OTHER
        // -------------------------------------------------

        case ISA::Opcode::CONV: this->executeConv(); break; // conv
    }
}

// -------------------------------------------------
// CORE
// -------------------------------------------------

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
// MEMORY
// -------------------------------------------------

void VM::executeLoad() {
    const uint8_t type = this->fetchType(); // read type operand
    const Value address = this->operandStack.pop(); // pop address from operand stack
    checkType("load", {static_cast<uint8_t>(ISA::Type::PTR), static_cast<uint8_t>(ISA::Type::UI32)}, static_cast<uint8_t>(address.type)); // ensure type of address is of type ptr or ui32

    const uint64_t value = this->memoryManager.read(MemoryAccessScope::PTR, address.rawValue, static_cast<ISA::Type>(type)); // read value at address
    this->operandStack.push(type, value); // push value onto operand stack
}

void VM::executeLoadB() {
    const Value address = this->operandStack.pop(); // pop address from operand stack
    checkType("loadB", {static_cast<uint8_t>(ISA::Type::PTR), static_cast<uint8_t>(ISA::Type::UI32)}, static_cast<uint8_t>(address.type)); // ensure type of address is of type ptr or ui32
    // read byte at address
    const uint8_t value = this->memoryManager.read8(MemoryAccessScope::PTR, address.rawValue);
    // push byte as ui32 onto the operand stack
    this->operandStack.push(static_cast<uint8_t>(ISA::Type::UI32), value);
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
        const uint64_t value = this->memoryManager.read(MemoryAccessScope::DATA, address, static_cast<ISA::Type>(valueDataType)); // read value at address
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

    // enforce offset is within current frame's bounds
    this->validateFrameAccess(offset);

    const uint32_t address = this->FP + (static_cast<int32_t>(offset) * 8); // calculate address from given operand immediate

    const uint64_t local = this->memoryManager.read64(MemoryAccessScope::CALL_STACK, address); // get local value from memory

    this->operandStack.push(static_cast<uint8_t>(type), local); // push local value onto operand stack
}

void VM::executeStore() {
    const Value value = this->operandStack.pop(); // pop value to store from operand stack
    const Value address = this->operandStack.pop(); // pop address to store to from operand stack
    checkType("store", {static_cast<uint8_t>(ISA::Type::PTR), static_cast<uint8_t>(ISA::Type::UI32)}, static_cast<uint8_t>(address.type)); // ensure type of address is of type ptr or ui32

    // store value in memory at address
    this->memoryManager.write(MemoryAccessScope::PTR, address.rawValue, &value);
}

void VM::executeStoreB() {
    const Value value = this->operandStack.pop(); // pop value to store from operand stack

    // enforce type of value is either i32, ui32, i64, or ui64
    checkType("storeB",
        {
        static_cast<uint8_t>(ISA::Type::I32),
        static_cast<uint8_t>(ISA::Type::UI32),
        static_cast<uint8_t>(ISA::Type::I64),
        static_cast<uint8_t>(ISA::Type::UI64),
        },
        static_cast<uint8_t>(value.type)
    );

    const Value address = this->operandStack.pop(); // pop address to store to from operand stack
    checkType("storeB", {static_cast<uint8_t>(ISA::Type::PTR), static_cast<uint8_t>(ISA::Type::UI32)}, static_cast<uint8_t>(address.type)); // ensure type of address is of type ptr or ui32

    // store 8 least significant bytes of value in memory at address
    this->memoryManager.write8(MemoryAccessScope::PTR, address.rawValue, value.rawValue & 0xFF);
}

void VM::executeStoreG() {
    const uint32_t address = this->fetchOperand(static_cast<uint8_t>(ISA::Type::PTR)); // read label operand
    const Value value = this->operandStack.pop(); // pop value from operand stack to store

    // ensure the value on the operand stack matches the type of the target global
    const uint8_t valueDataType = this->memoryManager.read8(MemoryAccessScope::DATA, address - 1); // read data type of target global
    checkType("storeG", {valueDataType}, static_cast<uint8_t>(value.type)); // ensure type of target global matches type of value from operand stack

    this->memoryManager.write(MemoryAccessScope::DATA, address, &value); // store val in memory at address
}

void VM::executeStoreL() {
    const uint64_t rawOffset = this->fetchOperand(static_cast<uint8_t>(ISA::Type::I32)); // read immediate operand
    const int32_t offset = TypeConversions::rawToI32(rawOffset);

    // enforce offset is within current frame's bounds
    this->validateFrameAccess(offset);

    const Value value = this->operandStack.pop(); // pop value from operand stack to store

    const uint32_t address = this->FP + (static_cast<int32_t>(offset) * 8); // calculate address from given operand immediate

    this->memoryManager.write64(MemoryAccessScope::CALL_STACK, address, value.rawValue); // store value from operand stack to memory
}

void VM::executeAlloc() {
    const Value value = this->operandStack.pop(); // pop number of bytes to allocate in the heap
    checkType("alloc", {static_cast<uint8_t>(ISA::Type::UI32)}, static_cast<uint8_t>(value.type));

    // allocate space on heap
    const uint32_t allocatedAddress = this->heapManager.allocateBlock(value.rawValue, this->SP);

    // push pointer onto the operand stack
    this->operandStack.push(static_cast<uint8_t>(ISA::Type::PTR), allocatedAddress);
}

void VM::executeFree() {
    const Value value = this->operandStack.pop(); // pop address of the block to deallocate from the heap
    checkType("free", {static_cast<uint8_t>(ISA::Type::PTR)}, static_cast<uint8_t>(value.type));
    // deallocate heap at address
    heapManager.deallocateBlock(value.rawValue);
}

// -------------------------------------------------
// CONTROL
// -------------------------------------------------

void VM::executeNative() {
    // read native function ID operand
    const uint8_t nativeID = this->memoryManager.read8(MemoryAccessScope::CODE, this->PC++);

    NativeFunctionManager::invoke(this, nativeID);
}

void VM::executeCall() {
    const uint32_t address = this->fetchOperand(static_cast<uint8_t>(ISA::Type::PTR)); // read label operand
    // read method metadata
    const uint8_t numberOfArguments = this->memoryManager.read8(MemoryAccessScope::CODE, address);
    const uint32_t numberOfLocals = this->memoryManager.read32(MemoryAccessScope::CODE, address + 1);

    // read arguments from operand stack
    std::vector<Value> arguments;
    for (int i = 0; i < numberOfArguments; i++) {
        arguments.push_back(this->operandStack.pop());
    }
    // push stack frame onto call stack
    this->callStackManager.push(this->FP, this->SP, this->PC, numberOfArguments, numberOfLocals, arguments, &this->HP);
    // set PC to start of called method
    this->PC = address + 5; // (5 for length of method metadata)
}

void VM::executeRet() {
    // pop stack frame off of call stack
    this->callStackManager.pop(this->FP, this->SP, this->PC);
}

void VM::executeJmp() {
    const uint32_t jumpAddress = this->fetchOperand(static_cast<uint8_t>(ISA::Type::PTR)); // read label operand
    this->PC = jumpAddress;
}

void VM::executeJez() {
    const uint32_t jumpAddress = this->fetchOperand(static_cast<uint8_t>(ISA::Type::PTR)); // read label operand
    const Value value = this->operandStack.pop(); // pop value off of the operand stack

    if (value.isZero()) {
        this->PC = jumpAddress;
    }
}

void VM::executeJnz() {
    const uint32_t jumpAddress = this->fetchOperand(static_cast<uint8_t>(ISA::Type::PTR)); // read label operand
    const Value value = this->operandStack.pop(); // pop value off of the operand stack

    if (!value.isZero()) {
        this->PC = jumpAddress;
    }
}

// -------------------------------------------------
// ARITHMETIC
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

void VM::executeCeq() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop();
    const Value value2 = this->operandStack.pop();

    // push 1 onto the operand stack if x == y, otherwise push 0
    this->operandStack.push(ArithmeticOps::ceq(value2, value1));
}

void VM::executeCne() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop();
    const Value value2 = this->operandStack.pop();

    // push 1 onto the operand stack if x != y, otherwise push 0
    this->operandStack.push(ArithmeticOps::cne(value2, value1));
}

void VM::executeClt() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop();
    const Value value2 = this->operandStack.pop();

    // push 1 onto the operand stack if x < y, otherwise push 0
    this->operandStack.push(ArithmeticOps::clt(value2, value1));
}

void VM::executeCle() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop();
    const Value value2 = this->operandStack.pop();

    // push 1 onto the operand stack if x <= y, otherwise push 0
    this->operandStack.push(ArithmeticOps::cle(value2, value1));
}

void VM::executeCgt() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop();
    const Value value2 = this->operandStack.pop();

    // push 1 onto the operand stack if x > y, otherwise push 0
    this->operandStack.push(ArithmeticOps::cgt(value2, value1));
}

void VM::executeCge() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop();
    const Value value2 = this->operandStack.pop();

    // push 1 onto the operand stack if x >= y, otherwise push 0
    this->operandStack.push(ArithmeticOps::cge(value2, value1));
}

void VM::executeConv() {
    const uint8_t type = this->fetchType(); // read type operand
    Value value = this->operandStack.pop(); // pop value off of the operand stack

    value.convertToType(static_cast<ISA::Type>(type)); // convert type
    this->operandStack.push(value); // push new value onto operand stack
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
    std::cerr << "HB: 0x" << std::hex << this->HB << "\n";
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

void VM::checkType(const std::string &instructionMnemonic, const std::vector<uint8_t> expectedTypes, const uint8_t actualType) {
    if (std::count(expectedTypes.begin(), expectedTypes.end(), actualType) == 0) {

        std::string string = "";

        string += "Error: type mismatch";
        string += "\nInstruction: " + instructionMnemonic;
        string += "\nExpected: ";
        for (const uint8_t expectedType : expectedTypes) {
            string += TypeConversions::typeToString(expectedType)  + " ";
        }
        string += "\nActual: " + TypeConversions::typeToString(actualType);

        throw VMError(string);
    }
}

void VM::validateFrameAccess(const int32_t offset) const {
    const FrameInfo* frameInfo = this->callStackManager.peekFrameInfo();
    if (frameInfo == nullptr ||
        (offset > 0 && offset > static_cast<int32_t>(frameInfo->numberOfArguments)) ||
        -offset > static_cast<int32_t>(frameInfo->numberOfLocals)) {

        throw VMError("ERROR: Invalid call stack access");
    }
}