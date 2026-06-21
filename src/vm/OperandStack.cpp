#include "OperandStack.h"

OperandStack::OperandStack() : stack(std::vector<Value>{}) {}

Value OperandStack::pop() {
    if (stack.empty()) {
        throw VMError("stack underflow (operand stack empty)");
    }
    Value top = stack.back();
    stack.pop_back();
    return top;
}

Value OperandStack::peek() const {
    if (stack.empty()) {
        throw VMError("stack underflow (operand stack empty)");
    }
    return stack.at(stack.size() - 1);
}

void OperandStack::push(const uint8_t typeOperand, const uint64_t rawValue) {
    if (stack.size() == MAX_OPERAND_STACK_SIZE) {
        throw VMError("stack overflow (max capacity reached)");
    }

    Value val;

    switch (typeOperand) {
        case 0x00: { // i32
            val = Value{ISA::Type::I32, rawValue};
            break;
        }
        case 0x01: { // i64
            val = Value{ISA::Type::UI32, rawValue};
            break;
        }
        case 0x02: { // ui32
            val = Value{ISA::Type::I64, rawValue};
            break;
        }
        case 0x03: { // ui64
            val = Value{ISA::Type::UI64, rawValue};
            break;
        }
        case 0x04: { // f32
            val = Value{ISA::Type::F32, rawValue};
            break;
        }
        case 0x05: { // f64
            val = Value{ISA::Type::F64, rawValue};
            break;
        }
        case 0x06: { // ptr
            val = Value{ISA::Type::PTR, rawValue};
            break;
        }
    }
    this->stack.push_back(val);
}

void OperandStack::push(const Value value) {
    this->stack.push_back(value);
}

const std::vector<Value>* OperandStack::getStack() const {
    return &this->stack;
}
