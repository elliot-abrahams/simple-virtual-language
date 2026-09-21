#ifndef SV_NATIVEFUNTIONMANAGER_H
#define SV_NATIVEFUNTIONMANAGER_H

#pragma once
#include <cstdint>

#include "OperandStack.h"
#include "VM.h"

class NativeFunctionManager {
public:
    static void invoke(VM* vm, const uint8_t id) {
        switch (id) {
            case 0x00: executeExit(vm); break;
            case 0x01: executePrint(vm); break;
            case 0x03: executePrintStr(vm); break;

            default:
                throw RuntimeError{
                    RuntimeErrorType::INTERNAL,
                    "native call ID " + std::to_string(id) + " does not correspond to a defined function"
                };
        }
    }

private:
    static void executeExit(VM* vm) {
        const Value exitStatus = vm->getOperandStack()->pop();

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

        vm->setExitStatus(static_cast<int>(exitStatus.rawValue));
    }

    static void executePrint(VM* vm) {
        const Value value = vm->getOperandStack()->pop();

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

        const std::string output = value.toString();

        if (output == "") {
            throw RuntimeError{
                RuntimeErrorType::INTERNAL,
                "invalid operand type for native function 'print'"
            };
        }

        std::cout << output;
    }

    static void executePrintStr(VM* vm) {
        const Value value = vm->getOperandStack()->pop();
        // type of value must be ptr
        vm->checkType("native print_str", {static_cast<uint8_t>(ISA::Type::PTR)}, static_cast<uint8_t>(value.type));

        std::cout << vm->readStringFromMemory(value.rawValue);
    }
};

#endif //SV_NATIVEFUNTIONMANAGER_H