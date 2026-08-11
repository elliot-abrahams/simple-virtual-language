#ifndef SV_NATIVEFUNTIONMANAGER_H
#define SV_NATIVEFUNTIONMANAGER_H

#pragma once
#include <cstdint>

#include "OperandStack.h"
#include "VM.h"

class NativeFunctionManager {
public:
    static void invoke(const uint8_t id, VM* vm, OperandStack& stack) {
        switch (id) {
            case 0: executeExit(vm, stack); break;

            default:
                throw VMError("Unknown native function ID");
        }
    }

private:
    static void executeExit(VM* vm, OperandStack& stack) {
        const Value exitStatus = stack.pop();
        vm->setExitStatus(exitStatus.toInt());
    }
};

#endif //SV_NATIVEFUNTIONMANAGER_H