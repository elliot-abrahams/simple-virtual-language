#ifndef SIMPLE_VM_VM_H
#define SIMPLE_VM_VM_H
#include <cstdint>
#include <vector>


class VM {

public:
    VM();

    void run(const std::vector<uint8_t> bytecode);

private:
    void execute();

    uint8_t PC;
    uint8_t HP;
    uint8_t FP;
    uint8_t SP;

    // operand stack
    // memory
};


#endif //SIMPLE_VM_VM_H