#ifndef SIMPLE_VM_DRIVER_H
#define SIMPLE_VM_DRIVER_H
#include <filesystem>
#include <string_view>


constexpr std::string_view BYTECODE_FILE_EXTENSION = ".svmb";
constexpr std::string_view ASSEMBLY_FILE_EXTENSION = ".svma";
constexpr std::string_view PROGRAMMING_LANGUAGE_FILE_EXTENSION = ".sv";

class Driver {

public:
    static int runCLI(int argc, char* argv[]);

private:
    static int run(const char* filePath);
    static int compile(const char* filePath);
    static int assemble(const char* filePath);
    static int execute(const char* filePath);

    static bool isValidFileExtension(const std::filesystem::path& filePath, const std::string_view* extension);

    static void handleUnsupportedFileExtensionError(const std::filesystem::path* path);
};


#endif //SIMPLE_VM_DRIVER_H