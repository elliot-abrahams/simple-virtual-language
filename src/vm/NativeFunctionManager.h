#ifndef SV_NATIVEFUNTIONMANAGER_H
#define SV_NATIVEFUNTIONMANAGER_H

#pragma once
#include <cstdint>

#include "OperandStack.h"
#include "VM.h"

class NativeFunctionManager {
public:
    static void invoke(VM* vm, std::optional<RuntimeError>* runtimeError, const uint8_t id) {
        switch (id) {
            case 0x00: executeExit(vm, runtimeError); break;
            case 0x01: executePrint(vm, runtimeError); break;
            case 0x03: executePrintStr(vm, runtimeError); break;

            default:
                *runtimeError = RuntimeError{
                    RuntimeErrorType::INTERNAL,
                    "native call ID " + std::to_string(id) + " does not correspond to a defined function"
                };
                throw std::runtime_error{""};
        }
    }

private:
    static void executeExit(VM* vm, std::optional<RuntimeError>* runtimeError) {
        const Value exitStatus = vm->getOperandStack()->pop(runtimeError);

        // type of exitStatus must be integer
        vm->checkType(
            "native exit",
            {
                static_cast<uint8_t>(ISA::Type::I32),
                static_cast<uint8_t>(ISA::Type::UI32),
                static_cast<uint8_t>(ISA::Type::I64),
                static_cast<uint8_t>(ISA::Type::UI64)
            },
            static_cast<uint8_t>(exitStatus.type));

        if (vm->getRuntimeError()->has_value()) return;

        vm->setExitStatus(static_cast<int>(exitStatus.rawValue));
    }

    static void executePrint(VM* vm, std::optional<RuntimeError>* runtimeError) {
        const Value value = vm->getOperandStack()->pop(runtimeError);

        // type of value must be either integer or float
        vm->checkType(
            "native print",
            {
                static_cast<uint8_t>(ISA::Type::I32),
                static_cast<uint8_t>(ISA::Type::UI32),
                static_cast<uint8_t>(ISA::Type::I64),
                static_cast<uint8_t>(ISA::Type::UI64),
                static_cast<uint8_t>(ISA::Type::F32),
                static_cast<uint8_t>(ISA::Type::F64)
            },
            static_cast<uint8_t>(value.type)
        );

        if (vm->getRuntimeError()->has_value()) return;

        const std::string output = value.toString();

        if (output == "") {
            *runtimeError = RuntimeError{
                RuntimeErrorType::INTERNAL,
                "invalid operand type for native function 'print'"
            };
            throw std::runtime_error{""};
        }

        std::cout << output;
    }

    static void executePrintStr(VM* vm, std::optional<RuntimeError>* runtimeError) {
        const Value value = vm->getOperandStack()->pop(runtimeError);
        // type of value must be ptr
        vm->checkType("native print_str", {static_cast<uint8_t>(ISA::Type::PTR)}, static_cast<uint8_t>(value.type));

        if (vm->getRuntimeError()->has_value()) return;

        std::cout << vm->readStringFromMemory(value.rawValue);
    }
};

#endif //SV_NATIVEFUNTIONMANAGER_H