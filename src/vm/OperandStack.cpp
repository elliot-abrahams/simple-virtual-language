#include "OperandStack.h"

#include "VM.h"
#include "VMError.h"

OperandStack::OperandStack() : stack(std::vector<Value>{}) {}

void OperandStack::pop() {
    if (stack.empty()) {
        throw VMError("stack underflow (operand stack empty)");
    }
    stack.pop_back();
}

TypedValue OperandStack::peek() const {
    if (stack.empty()) {
        throw VMError("stack underflow (operand stack empty)");
    }
    return stack.at(stack.size() - 1).toTyped();
}

void OperandStack::push(const uint8_t typeOperand, const uint64_t rawValue) {
    if (stack.size() == MAX_OPERAND_STACK_SIZE) {
        throw VMError("stack overflow (max capacity reached)");
    }

    Value val;

    switch (typeOperand) {
        case 0x00: { // i32
            val = Value{Type::I32, rawValue};
            break;
        }
        case 0x01: { // i64
            val = Value{Type::I64, rawValue};
            break;
        }
        case 0x02: { // ui32
            val = Value{Type::UI32, rawValue};
            break;
        }
        case 0x03: { // ui64
            val = Value{Type::UI64, rawValue};
            break;
        }
        case 0x04: { // f32
            val = Value{Type::F32, rawValue};
            break;
        }
        case 0x05: { // f64
            val = Value{Type::F64, rawValue};
            break;
        }
        case 0x06: { // ptr
            val = Value{Type::PTR, rawValue};
            break;
        }
        case 0x07: { // char
            val = Value{Type::CHAR, rawValue};
            break;
        }
    }
    this->stack.push_back(val);
}

const std::vector<Value>* OperandStack::getStack() const {
    return &this->stack;
}
