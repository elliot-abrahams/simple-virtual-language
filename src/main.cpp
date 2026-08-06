#pragma once

#define NOMINMAX
#include <windows.h>

#include "Driver.h"

int main(int argc, char* argv[]) {
    // set output to UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    return Driver::runCLI(argc, argv);
}
