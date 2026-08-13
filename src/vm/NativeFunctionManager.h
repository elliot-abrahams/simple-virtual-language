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
                throw VMError("Unknown native function ID");
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
        vm->setExitStatus(exitStatus.toInt());
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
            static_cast<uint8_t>(value.type));

        switch (static_cast<ISA::Type>(value.type)) {
            case ISA::Type::UI32:
            case ISA::Type::UI64:
                std::cout << value.rawValue;
                break;

            case ISA::Type::I32: std::cout << TypeConversions::rawToI32(value.rawValue); break;
            case ISA::Type::I64: std::cout << TypeConversions::rawToI64(value.rawValue); break;
            case ISA::Type::F32: std::cout << formatFloatString(TypeConversions::rawToF32(value.rawValue)); break;
            case ISA::Type::F64: std::cout << formatFloatString(TypeConversions::rawToF64(value.rawValue)); break;

            default:
                throw VMError("Incorrect operand type for native function 'print'");
        }
    }

    static void executePrintStr(VM* vm) {
        const Value value = vm->getOperandStack()->pop();
        // type of value must be ptr
        vm->checkType("native print_str", {static_cast<uint8_t>(ISA::Type::PTR)}, static_cast<uint8_t>(value.type));

        std::cout << vm->readStringFromMemory(value.rawValue);
    }

    static std::string formatFloatString(const double value) {
        std::ostringstream oss;
        oss << std::setprecision(std::numeric_limits<double>::max_digits10)
            << std::defaultfloat
            << value;

        std::string formattedString = oss.str();

        auto dot = formattedString.find('.');

        if (dot == std::string::npos) {
            // no decimal point and not scientific notation
            if (formattedString.find('e') == std::string::npos &&
                formattedString.find('E') == std::string::npos) {

                formattedString += ".0";
                }

        } else {
            while (formattedString.back() == '0') {
                // remove trailing zeros
                formattedString.pop_back();
            }

            if (formattedString.back() == '.') {
                formattedString.push_back('0');
            }
        }

        return formattedString;
    }
};

#endif //SV_NATIVEFUNTIONMANAGER_H