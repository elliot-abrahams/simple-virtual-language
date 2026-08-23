#include "VM.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <ios>
#include <iostream>

#include "TypeConversions.h"
#include "ArithmeticOps.h"
#include "NativeFunctionManager.h"
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
    runtimeErrorHandler(RuntimeErrorHandler(this)),
    running(true),
    exitCode(0) {}

void VM::run(const std::vector<uint8_t>* bytecode) {
    // read bytecode
    this->readBytecode(bytecode);

    // set HB
    this->HB = bytecode->size() - BYTECODE_HEADER_SIZE;
    this->HP = this->HB;

    this->heapManager.initialiseHeap(&this->HP);

    Value::runtimeError = &this->runtimeError;

    while (running) {
        this->execute();

        if (this->runtimeError.has_value()) {
            this->exitCode = 1;
            return;
        }

        if (this->exitCode != 0) return;
    }
}

void VM::readBytecode(const std::vector<uint8_t> *bytecode) {
    // read first header field (end of code address)
    uint32_t endOfCodeAddress = 0;
    for (int i = 0; i < 4; i++) {
        endOfCodeAddress = endOfCodeAddress | static_cast<uint32_t>((*bytecode)[i]) << (i * 8);
    }

    // read second header field (end of data address)
    uint32_t endOfDataAddress = 0;
    for (int i = 0; i < 4; i++) {
        endOfDataAddress = endOfDataAddress | static_cast<uint32_t>((*bytecode)[i + 4]) << (i * 8);
    }

    // read third header field (end of sources address)
    uint32_t endOfSourcesAddress = 0;
    for (int i = 0; i < 4; i++) {
        endOfSourcesAddress = endOfSourcesAddress | static_cast<uint32_t>((*bytecode)[i + 8]) << (i * 8);
    }

    // read fourth header field (end of line table address)
    uint32_t endOfLineTableAddress = 0;
    for (int i = 0; i < 4; i++) {
        endOfLineTableAddress = endOfLineTableAddress | static_cast<uint32_t>((*bytecode)[i + 12]) << (i * 8);
    }

    ErrorContext errorContext;
    if (endOfDataAddress == endOfSourcesAddress && endOfSourcesAddress == endOfLineTableAddress) {
        // if debug information not present
        errorContext = ErrorContext::INTERNAL;
    } else {
        errorContext = ErrorContext::LANGUAGE;
    }
    this->errorContext = errorContext;
    Value::errorContext = errorContext;

    // load bytecode into memory
    this->memoryManager.loadBytecodeIntoMemory(&this->runtimeError, bytecode, endOfDataAddress);
    this->memoryManager.setStartOfDataRegion(endOfCodeAddress + 1 - BYTECODE_HEADER_SIZE);

    // decode debug information for runtimeErrorHandler

    // traverse debug source info in bytecode
    uint32_t bytecodeIdx = endOfDataAddress + 1;
    while (bytecodeIdx < endOfSourcesAddress) {
        // read sourceId from bytecode
        uint16_t sourceId = 0;
        for (int i = 0; i < 2; i++) {
            sourceId = sourceId | static_cast<uint32_t>((*bytecode)[bytecodeIdx + i]) << (i * 8);
        }
        bytecodeIdx += 2;

        // read source path
        uint32_t pathStringSize = 0;
        for (int i = 0; i < 4; i++) {
            pathStringSize = pathStringSize | static_cast<uint32_t>((*bytecode)[bytecodeIdx + i]) << (i * 8);
        }
        bytecodeIdx += 4;

        std::string path;
        path.resize(pathStringSize);

        for (int i = 0; i < pathStringSize; i++) {
            path[i] = static_cast<char>((*bytecode)[bytecodeIdx + i]);
        }

        bytecodeIdx += pathStringSize;

        this->runtimeErrorHandler.insertSource(sourceId, path);
    }

    // traverse debug line table in bytecode
    bytecodeIdx = endOfSourcesAddress + 1;
    while (bytecodeIdx < endOfLineTableAddress) {
        // read start address
        uint32_t startAddress = 0;
        for (int i = 0; i < 4; i++) {
            startAddress = startAddress | static_cast<uint32_t>((*bytecode)[bytecodeIdx + i]) << (i * 8);
        }
        bytecodeIdx += 4;

        // read end address
        uint32_t endAddress = 0;
        for (int i = 0; i < 4; i++) {
            endAddress = endAddress | static_cast<uint32_t>((*bytecode)[bytecodeIdx + i]) << (i * 8);
        }
        bytecodeIdx += 4;

        // read source id
        uint32_t sourceId = 0;
        for (int i = 0; i < 2; i++) {
            sourceId = sourceId | static_cast<uint32_t>((*bytecode)[bytecodeIdx + i]) << (i * 8);
        }
        bytecodeIdx += 2;

        // read line number
        uint32_t lineNumber = 0;
        for (int i = 0; i < 4; i++) {
            lineNumber = lineNumber | static_cast<uint32_t>((*bytecode)[bytecodeIdx + i]) << (i * 8);
        }
        bytecodeIdx += 4;

        // read column number
        uint32_t columnNumber = 0;
        for (int i = 0; i < 2; i++) {
            columnNumber = columnNumber | static_cast<uint32_t>((*bytecode)[bytecodeIdx + i]) << (i * 8);
        }
        bytecodeIdx += 2;

        this->runtimeErrorHandler.insertDebugLine(
            startAddress,
            endAddress,
            sourceId,
            lineNumber,
            columnNumber
        );
    }

    // set Heap Base register to the end of the data region + 1
    this->HB =  endOfDataAddress + 1 - BYTECODE_HEADER_SIZE;
}

Value VM::popOperandStack() {
    return this->operandStack.pop(&this->runtimeError);
}

OperandStack* VM::getOperandStack() {
    return &this->operandStack;
}

CallStackManager* VM::getCallStackManager() {
    return &this->callStackManager;
}

void VM::handleRuntimeError() const {
    if (!this->runtimeError.has_value()) {
        return;
    }

    this->runtimeErrorHandler.raiseRuntimeError(*runtimeError);
}

void VM::setExitStatus(const int status) {
    this->exitCode = status;
}

int VM::getExitStatus() const {
    return this->exitCode;
}

uint32_t VM::getPC() const {
    return this->PC;
}

ErrorContext VM::getErrorContext() const {
    return this->errorContext;
}

const std::optional<RuntimeError>* VM::getRuntimeError() const {
    return &this->runtimeError;
}

std::string VM::readStringFromMemory(const uint32_t address) {
    // read number of bytes of string
    const uint32_t size = this->memoryManager.read32(&this->runtimeError, MemoryAccessScope::PTR, address);

    std::string string;
    string.resize(size);

    for (int i = 0; i < size; i++) {
        string[i] = static_cast<char>(this->memoryManager.read8(&this->runtimeError, MemoryAccessScope::PTR, address + 4 + i));
    }
    return string;
}

void VM::execute() {
    // read byte at PC
    const uint8_t opcode = this->memoryManager.read8(&this->runtimeError, MemoryAccessScope::CODE, this->PC++);

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
    this->operandStack.push(&this->runtimeError, type, value); // push value onto the operand stack
}

void VM::executePop() {
    this->operandStack.pop(&this->runtimeError); // remove top of stack
}

void VM::executeDup() {
    const Value val = this->operandStack.peek(&this->runtimeError); // get top value of operand stack
    this->operandStack.push(&this->runtimeError, static_cast<uint8_t>(val.type), val.rawValue); // push that value onto the operand stack
}

void VM::executeSwap() {
    // pop top two values from operand stack
    const Value val1 = this->operandStack.pop(&this->runtimeError);
    const Value val2 = this->operandStack.pop(&this->runtimeError);

    // push the two values onto the operand stack but swapped
    this->operandStack.push(&this->runtimeError, static_cast<uint8_t>(val1.type), val1.rawValue);
    this->operandStack.push(&this->runtimeError, static_cast<uint8_t>(val2.type), val2.rawValue);
}

// -------------------------------------------------
// MEMORY
// -------------------------------------------------

void VM::executeLoad() {
    const uint8_t type = this->fetchType(); // read type operand
    const Value address = this->operandStack.pop(&this->runtimeError); // pop address from operand stack
    this->checkType("load", {static_cast<uint8_t>(ISA::Type::PTR), static_cast<uint8_t>(ISA::Type::UI32)}, static_cast<uint8_t>(address.type)); // ensure type of address is of type ptr or ui32
    if (this->runtimeError.has_value()) return;


    const uint64_t value = this->memoryManager.read(&this->runtimeError, MemoryAccessScope::PTR, address.rawValue, static_cast<ISA::Type>(type)); // read value at address
    this->operandStack.push(&this->runtimeError, type, value); // push value onto operand stack
}

void VM::executeLoadB() {
    const Value address = this->operandStack.pop(&this->runtimeError); // pop address from operand stack
    this->checkType("loadB", {static_cast<uint8_t>(ISA::Type::PTR), static_cast<uint8_t>(ISA::Type::UI32)}, static_cast<uint8_t>(address.type)); // ensure type of address is of type ptr or ui32
    if (this->runtimeError.has_value()) return;

    // read byte at address
    const uint8_t value = this->memoryManager.read8(&this->runtimeError, MemoryAccessScope::PTR, address.rawValue);
    // push byte as ui32 onto the operand stack
    this->operandStack.push(&this->runtimeError, static_cast<uint8_t>(ISA::Type::UI32), value);
}

void VM::executeLoadG() {
    const uint32_t address = this->fetchOperand(static_cast<uint8_t>(ISA::Type::PTR)); // read label operand
    const uint8_t valueDataType = this->memoryManager.read8(&this->runtimeError, MemoryAccessScope::DATA, address - 1); // load data type at address

    /*
     * <data type of global at address>
     * STR -> push address onto operand stack
     * NOT STR -> push value at address onto operand stack
    */

    if (valueDataType != static_cast<uint8_t>(ISA::Type::STR)) {
        const uint64_t value = this->memoryManager.read(&this->runtimeError, MemoryAccessScope::DATA, address, static_cast<ISA::Type>(valueDataType)); // read value at address
        this->operandStack.push(&this->runtimeError, valueDataType, value); // push value onto operand stack
    } else {
        // push address (pointer to the string) onto operand stack
        this->operandStack.push(&this->runtimeError, valueDataType, address);
    }
}

void VM::executeLoadL() {
    const uint8_t type = this->fetchType(); // read type operand
    const uint64_t rawOffset = this->fetchOperand(static_cast<uint8_t>(ISA::Type::I32)); // read immediate operand
    const int32_t offset = TypeConversions::rawToI32(rawOffset);

    // enforce offset is within current frame's bounds
    this->validateFrameAccess(offset);

    const uint32_t address = this->FP + (static_cast<int32_t>(offset) * 8); // calculate address from given operand immediate

    const uint64_t local = this->memoryManager.read64(&this->runtimeError, MemoryAccessScope::CALL_STACK, address); // get local value from memory

    this->operandStack.push(&this->runtimeError, static_cast<uint8_t>(type), local); // push local value onto operand stack
}

void VM::executeStore() {
    const Value value = this->operandStack.pop(&this->runtimeError); // pop value to store from operand stack
    const Value address = this->operandStack.pop(&this->runtimeError); // pop address to store to from operand stack
    this->checkType("store", {static_cast<uint8_t>(ISA::Type::PTR), static_cast<uint8_t>(ISA::Type::UI32)}, static_cast<uint8_t>(address.type)); // ensure type of address is of type ptr or ui32
    if (this->runtimeError.has_value()) return;

    // store value in memory at address
    this->memoryManager.write(&this->runtimeError, MemoryAccessScope::PTR, address.rawValue, &value);
}

void VM::executeStoreB() {
    const Value value = this->operandStack.pop(&this->runtimeError); // pop value to store from operand stack

    // enforce type of value is either i32, ui32, i64, or ui64
    this->checkType("storeB",
        {
        static_cast<uint8_t>(ISA::Type::I32),
        static_cast<uint8_t>(ISA::Type::UI32),
        static_cast<uint8_t>(ISA::Type::I64),
        static_cast<uint8_t>(ISA::Type::UI64),
        },
        static_cast<uint8_t>(value.type)
    );
    if (this->runtimeError.has_value()) return;

    const Value address = this->operandStack.pop(&this->runtimeError); // pop address to store to from operand stack
    this->checkType("storeB", {static_cast<uint8_t>(ISA::Type::PTR), static_cast<uint8_t>(ISA::Type::UI32)}, static_cast<uint8_t>(address.type)); // ensure type of address is of type ptr or ui32
    if (this->runtimeError.has_value()) return;

    // store 8 least significant bytes of value in memory at address
    this->memoryManager.write8(&this->runtimeError, MemoryAccessScope::PTR, address.rawValue, value.rawValue & 0xFF);
}

void VM::executeStoreG() {
    const uint32_t address = this->fetchOperand(static_cast<uint8_t>(ISA::Type::PTR)); // read label operand
    const Value value = this->operandStack.pop(&this->runtimeError); // pop value from operand stack to store

    // ensure the value on the operand stack matches the type of the target global
    const uint8_t valueDataType = this->memoryManager.read8(&this->runtimeError, MemoryAccessScope::DATA, address - 1); // read data type of target global
    this->checkType("storeG", {valueDataType}, static_cast<uint8_t>(value.type)); // ensure type of target global matches type of value from operand stack
    if (this->runtimeError.has_value()) return;

    this->memoryManager.write(&this->runtimeError, MemoryAccessScope::DATA, address, &value); // store val in memory at address
}

void VM::executeStoreL() {
    const uint64_t rawOffset = this->fetchOperand(static_cast<uint8_t>(ISA::Type::I32)); // read immediate operand
    const int32_t offset = TypeConversions::rawToI32(rawOffset);

    // enforce offset is within current frame's bounds
    this->validateFrameAccess(offset);

    const Value value = this->operandStack.pop(&this->runtimeError); // pop value from operand stack to store

    const uint32_t address = this->FP + (static_cast<int32_t>(offset) * 8); // calculate address from given operand immediate

    this->memoryManager.write64(&this->runtimeError, MemoryAccessScope::CALL_STACK, address, value.rawValue); // store value from operand stack to memory
}

void VM::executeAlloc() {
    const Value value = this->operandStack.pop(&this->runtimeError); // pop number of bytes to allocate in the heap
    this->checkType("alloc", {static_cast<uint8_t>(ISA::Type::UI32)}, static_cast<uint8_t>(value.type));
    if (this->runtimeError.has_value()) return;

    // allocate space on heap
    const uint32_t allocatedAddress = this->heapManager.allocateBlock(&this->runtimeError, value.rawValue, this->SP);

    // push pointer onto the operand stack
    this->operandStack.push(&this->runtimeError, static_cast<uint8_t>(ISA::Type::PTR), allocatedAddress);
}

void VM::executeFree() {
    const Value value = this->operandStack.pop(&this->runtimeError); // pop address of the block to deallocate from the heap
    this->checkType("free", {static_cast<uint8_t>(ISA::Type::PTR)}, static_cast<uint8_t>(value.type));
    if (this->runtimeError.has_value()) return;

    // deallocate heap at address
    heapManager.deallocateBlock(&this->runtimeError, value.rawValue);
}

// -------------------------------------------------
// CONTROL
// -------------------------------------------------

void VM::executeNative() {
    // read native function ID operand
    const uint8_t nativeID = this->memoryManager.read8(&this->runtimeError, MemoryAccessScope::CODE, this->PC++);

    NativeFunctionManager::invoke(this, &this->runtimeError,  nativeID);
}

void VM::executeCall() {
    const uint32_t address = this->fetchOperand(static_cast<uint8_t>(ISA::Type::PTR)); // read label operand
    // read method metadata
    const uint8_t numberOfArguments = this->memoryManager.read8(&this->runtimeError, MemoryAccessScope::CODE, address);
    const uint32_t numberOfLocals = this->memoryManager.read32(&this->runtimeError, MemoryAccessScope::CODE, address + 1);

    // read arguments from operand stack
    std::vector<Value> arguments;
    for (int i = 0; i < numberOfArguments; i++) {
        arguments.push_back(this->operandStack.pop(&this->runtimeError));
    }
    // push stack frame onto call stack
    this->callStackManager.push(&this->runtimeError, this->FP, this->SP, this->PC, numberOfArguments, numberOfLocals, arguments, &this->HP, this->PC);
    if (this->runtimeError.has_value()) return;

    // set PC to start of called method
    this->PC = address + 5; // (5 for length of method metadata)
}

void VM::executeRet() {
    // pop stack frame off of call stack
    this->callStackManager.pop(&this->runtimeError, this->FP, this->SP, this->PC);
}

void VM::executeJmp() {
    const uint32_t jumpAddress = this->fetchOperand(static_cast<uint8_t>(ISA::Type::PTR)); // read label operand
    this->PC = jumpAddress;
}

void VM::executeJez() {
    const uint32_t jumpAddress = this->fetchOperand(static_cast<uint8_t>(ISA::Type::PTR)); // read label operand
    const Value value = this->operandStack.pop(&this->runtimeError); // pop value off of the operand stack

    if (value.isZero()) {
        this->PC = jumpAddress;
    }
}

void VM::executeJnz() {
    const uint32_t jumpAddress = this->fetchOperand(static_cast<uint8_t>(ISA::Type::PTR)); // read label operand
    const Value value = this->operandStack.pop(&this->runtimeError); // pop value off of the operand stack

    if (!value.isZero()) {
        this->PC = jumpAddress;
    }
}

// -------------------------------------------------
// ARITHMETIC
// -------------------------------------------------

void VM::executeAdd() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop(&this->runtimeError);
    const Value value2 = this->operandStack.pop(&this->runtimeError);

    // add those two values and push the result onto the operand stack
    const Value result = ArithmeticOps::add(&this->runtimeError, value2, value1);
    if (this->runtimeError.has_value()) return;
    this->operandStack.push(&this->runtimeError, result);
}

void VM::executeSub() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop(&this->runtimeError);
    const Value value2 = this->operandStack.pop(&this->runtimeError);

    // subtract those two values and push the result onto the operand stack
    const Value result = ArithmeticOps::sub(&this->runtimeError, value2, value1);
    if (this->runtimeError.has_value()) return;
    this->operandStack.push(&this->runtimeError, result);
}

void VM::executeMul() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop(&this->runtimeError);
    const Value value2 = this->operandStack.pop(&this->runtimeError);

    // multiply those two values and push the result onto the operand stack
    const Value result = ArithmeticOps::mul(&this->runtimeError, value2, value1);
    if (this->runtimeError.has_value()) return;
    this->operandStack.push(&this->runtimeError, result);
}

void VM::executeDiv() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop(&this->runtimeError);
    const Value value2 = this->operandStack.pop(&this->runtimeError);

    if (value1.isZero()) {
        this->runtimeError = RuntimeError{
            RuntimeErrorType::DIVISION_BY_ZERO,
            "division by zero"
        };
        return;
    }

    // multiply those two values and push the result onto the operand stack
    const Value result = ArithmeticOps::div(&this->runtimeError, value2, value1);
    if (this->runtimeError.has_value()) return;
    this->operandStack.push(&this->runtimeError, result);
}

void VM::executeMod() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop(&this->runtimeError);
    const Value value2 = this->operandStack.pop(&this->runtimeError);

    if (value1.isZero()) {
        this->runtimeError = RuntimeError{
            RuntimeErrorType::DIVISION_BY_ZERO,
            "modulo by zero"
        };
        return;
    }

    // compute modulo and push the result onto the operand stack
    const Value result = ArithmeticOps::mod(&this->runtimeError, value2, value1);
    if (this->runtimeError.has_value()) return;
    this->operandStack.push(&this->runtimeError, result);
}

void VM::executeNot() {
    const Value value = this->operandStack.pop(&this->runtimeError); // pop value from the operand stack

    // compute bitwise not and push the result onto the operand stack
    const Value result = ArithmeticOps::bitwiseNot(&this->runtimeError, value);
    if (this->runtimeError.has_value()) return;
    this->operandStack.push(&this->runtimeError, result);
}

void VM::executeAnd() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop(&this->runtimeError);
    const Value value2 = this->operandStack.pop(&this->runtimeError);

    // compute bitwise and, and push the result onto the operand stack
    const Value result = ArithmeticOps::bitwiseAnd(&this->runtimeError, value2, value1);
    if (this->runtimeError.has_value()) return;
    this->operandStack.push(&this->runtimeError, result);
}

void VM::executeOrr() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop(&this->runtimeError);
    const Value value2 = this->operandStack.pop(&this->runtimeError);

    // compute bitwise or, and push the result onto the operand stack
    const Value result = ArithmeticOps::bitwiseOr(&this->runtimeError, value2, value1);
    if (this->runtimeError.has_value()) return;
    this->operandStack.push(&this->runtimeError, result);
}

void VM::executeXor() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop(&this->runtimeError);
    const Value value2 = this->operandStack.pop(&this->runtimeError);

    // compute bitwise xor and push the result onto the operand stack
    const Value result = ArithmeticOps::bitwiseXor(&this->runtimeError, value2, value1);
    if (this->runtimeError.has_value()) return;
    this->operandStack.push(&this->runtimeError, result);
}

void VM::executeShl() {
    // pop two values off of the operand stack
    const Value numberOfShifts = this->operandStack.pop(&this->runtimeError);
    const Value value = this->operandStack.pop(&this->runtimeError);

    // compute logical shift left and push the result onto the operand stack
    const Value result = ArithmeticOps::shl(&this->runtimeError, value, numberOfShifts);
    if (this->runtimeError.has_value()) return;
    this->operandStack.push(&this->runtimeError, result);
}

void VM::executeShr() {
    // pop two values off of the operand stack
    const Value numberOfShifts = this->operandStack.pop(&this->runtimeError);
    const Value value = this->operandStack.pop(&this->runtimeError);

    // compute logical shift right and push the result onto the operand stack
    const Value result = ArithmeticOps::shr(&this->runtimeError, false, value, numberOfShifts);
    if (this->runtimeError.has_value()) return;
    this->operandStack.push(&this->runtimeError, result);
}

void VM::executeSar() {
    // pop two values off of the operand stack
    const Value numberOfShifts = this->operandStack.pop(&this->runtimeError);
    const Value value = this->operandStack.pop(&this->runtimeError);

    // compute arithmetic shift right and push the result onto the operand stack
    const Value result = ArithmeticOps::shr(&this->runtimeError, true, value, numberOfShifts);
    if (this->runtimeError.has_value()) return;
    this->operandStack.push(&this->runtimeError, result);
}

void VM::executeCeq() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop(&this->runtimeError);
    const Value value2 = this->operandStack.pop(&this->runtimeError);

    // push 1 onto the operand stack if x == y, otherwise push 0
    const Value result = ArithmeticOps::ceq(&this->runtimeError, value2, value1);
    if (this->runtimeError.has_value()) return;
    this->operandStack.push(&this->runtimeError, result);
}

void VM::executeCne() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop(&this->runtimeError);
    const Value value2 = this->operandStack.pop(&this->runtimeError);

    // push 1 onto the operand stack if x != y, otherwise push 0
    const Value result = ArithmeticOps::cne(&this->runtimeError, value2, value1);
    if (this->runtimeError.has_value()) return;
    this->operandStack.push(&this->runtimeError, result);
}

void VM::executeClt() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop(&this->runtimeError);
    const Value value2 = this->operandStack.pop(&this->runtimeError);

    // push 1 onto the operand stack if x < y, otherwise push 0
    const Value result = ArithmeticOps::clt(&this->runtimeError, value2, value1);
    if (this->runtimeError.has_value()) return;
    this->operandStack.push(&this->runtimeError, result);
}

void VM::executeCle() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop(&this->runtimeError);
    const Value value2 = this->operandStack.pop(&this->runtimeError);

    // push 1 onto the operand stack if x <= y, otherwise push 0
    const Value result = ArithmeticOps::cle(&this->runtimeError, value2, value1);
    if (this->runtimeError.has_value()) return;
    this->operandStack.push(&this->runtimeError, result);
}

void VM::executeCgt() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop(&this->runtimeError);
    const Value value2 = this->operandStack.pop(&this->runtimeError);

    // push 1 onto the operand stack if x > y, otherwise push 0
    const Value result = ArithmeticOps::cgt(&this->runtimeError, value2, value1);
    if (this->runtimeError.has_value()) return;
    this->operandStack.push(&this->runtimeError, result);
}

void VM::executeCge() {
    // pop two values off of the operand stack
    const Value value1 = this->operandStack.pop(&this->runtimeError);
    const Value value2 = this->operandStack.pop(&this->runtimeError);

    // push 1 onto the operand stack if x >= y, otherwise push 0
    const Value result = ArithmeticOps::cge(&this->runtimeError, value2, value1);
    if (this->runtimeError.has_value()) return;
    this->operandStack.push(&this->runtimeError, result);
}

void VM::executeConv() {
    const uint8_t type = this->fetchType(); // read type operand
    Value value = this->operandStack.pop(&this->runtimeError); // pop value off of the operand stack

    value.convertToType(static_cast<ISA::Type>(type)); // convert type
    if (this->runtimeError.has_value()) return;
    this->operandStack.push(&this->runtimeError, value); // push new value onto operand stack
}

uint8_t VM::fetchType() {
    return this->memoryManager.read8(&this->runtimeError, MemoryAccessScope::CODE, this->PC++);
}

uint64_t VM::fetchOperand(const uint8_t type) {
    uint64_t result = 0;
    switch (static_cast<ISA::Type>(type)) {
        case ISA::Type::I32: // i32
        case ISA::Type::UI32: // ui32
        case ISA::Type::F32: // f32
        case ISA::Type::PTR: // ptr
        {
            result = static_cast<uint64_t>(this->memoryManager.read32(&this->runtimeError, MemoryAccessScope::CODE, this->PC));
            this->PC += 4;
            break;
        }
        case ISA::Type::I64: // i64
        case ISA::Type::UI64: // ui64
        case ISA::Type::F64: // f64
        {
            result = this->memoryManager.read64(&this->runtimeError, MemoryAccessScope::CODE, this->PC);
            this->PC += 8;
            break;
        }
    }
    return result;
}

void VM::dumpState() const {
    const auto& stack = this->operandStack.getStack();

    std::cerr << "--- VM STATE ---\n";

    std::cerr << "  PC: 0x" << std::hex << this->PC << "\n";
    std::cerr << "  HB: 0x" << std::hex << this->HB << "\n";
    std::cerr << "  FP: 0x" << std::hex << this->FP << "\n";
    std::cerr << "  SP: 0x" << std::hex << this->SP << "\n";

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

        std::string errorMessage = "invalid operand type for '";
        errorMessage += instructionMnemonic;
        errorMessage += "'. expected type(s) ";

        for (const uint8_t expectedType : expectedTypes) {
            errorMessage += TypeConversions::typeToString(expectedType) + ", ";
        }

        errorMessage += "but found ";
        errorMessage += TypeConversions::typeToString(actualType);

        runtimeError = RuntimeError{
            RuntimeErrorType::INTERNAL,
            errorMessage
        };
    }
}

void VM::validateFrameAccess(const int32_t offset) {
    const FrameInfo* frameInfo = this->callStackManager.peekFrameInfo();
    if (frameInfo == nullptr) {
        this->runtimeError = RuntimeError{
            RuntimeErrorType::INTERNAL,
            "attempted to access a local variable or argument outside the current call frame"
        };
    } else if (offset > 0 && offset > static_cast<int32_t>(frameInfo->numberOfArguments) ||
            -offset > static_cast<int32_t>(frameInfo->numberOfLocals)
    ) {
        this->runtimeError = RuntimeError{
            RuntimeErrorType::INTERNAL,
            "attempted to access a local variable or argument without an active call frame"
        };
    }
}