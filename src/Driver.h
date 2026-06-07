#ifndef SIMPLE_VM_DRIVER_H
#define SIMPLE_VM_DRIVER_H
#include <filesystem>
#include <string_view>


constexpr std::string_view BYTECODE_FILE_EXTENSION = ".svmb";
constexpr std::string_view ASSEMBLY_FILE_EXTENSION = ".svma";

class Driver {

public:
    static int runCLI(int argc, char* argv[]);

private:
    static int assemble(const char* filePath);
    static int execute(const char* filePath);

    static void handleUnsupportedFileExtensionError(const std::filesystem::path* path);
};


#endif //SIMPLE_VM_DRIVER_H