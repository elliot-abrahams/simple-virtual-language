#include "OperandStack.h"

OperandStack::OperandStack() : stack(std::vector<Value>{}) {}

Value OperandStack::pop(std::optional<RuntimeError>* runtimeError) {
    if (this->stack.empty()) {
        *runtimeError = RuntimeError {
            RuntimeErrorType::INTERNAL,
            "attempted to pop from an empty operand stack"
        };
        return Value{};
    }
    const Value top = this->stack.back();
    this->stack.pop_back();
    return top;
}

Value OperandStack::peek(std::optional<RuntimeError>* runtimeError) const {
    if (this->stack.empty()) {
        *runtimeError = RuntimeError {
            RuntimeErrorType::INTERNAL,
            "attempted to peek at an empty operand stack"
        };
        return Value{};
    }
    return this->stack.at(stack.size() - 1);
}

void OperandStack::push(std::optional<RuntimeError>* runtimeError, const uint8_t typeOperand, const uint64_t rawValue) {
    if (this->stack.size() == MAX_OPERAND_STACK_SIZE) {
        *runtimeError = RuntimeError {
            RuntimeErrorType::INTERNAL,
            "attempted to push onto a full operand stack"
        };
        return;
    }

    Value val{};

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

void OperandStack::push(std::optional<RuntimeError>* runtimeError, const Value value) {
    if (this->stack.size() == MAX_OPERAND_STACK_SIZE) {
        *runtimeError = RuntimeError {
            RuntimeErrorType::INTERNAL,
            "attempted to push onto a full operand stack"
        };
        return;
    }

    this->stack.push_back(value);
}

const std::vector<Value>* OperandStack::getStack() const {
    return &this->stack;
}
