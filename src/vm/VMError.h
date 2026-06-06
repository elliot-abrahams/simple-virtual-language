#ifndef SIMPLE_VM_VMERROR_H
#define SIMPLE_VM_VMERROR_H

#include <stdexcept>
#include <string>

class VMError : public std::runtime_error
{
public:
    explicit VMError(const std::string& message) : std::runtime_error(message) {}
};

#endif //SIMPLE_VM_VMERROR_H