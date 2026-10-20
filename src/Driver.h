#ifndef SIMPLE_VM_DRIVER_H
#define SIMPLE_VM_DRIVER_H
#include <filesystem>
#include <string_view>
#include <vector>


constexpr std::string_view BYTECODE_FILE_EXTENSION = ".svmb";
constexpr std::string_view ASSEMBLY_FILE_EXTENSION = ".svma";
constexpr std::string_view PROGRAMMING_LANGUAGE_FILE_EXTENSION = ".sv";


namespace driver {
    enum class CLICommand {
        HELP,
        EXECUTE,
        ASSEMBLE,
        COMPILE,
        RUN
    };

    enum class OptimisationLevel {
        O0,
        DEFAULT
    };

    struct CLICommandOptions {
        bool outputBytecode = false;
        bool outputAssembly = false;
        OptimisationLevel optimisationLevel = OptimisationLevel::DEFAULT;
    };

    class Driver {

    public:
        static int runCLI(int argc, char* argv[]);

    private:
        static CLICommand readCommand(const std::string& command);
        static CLICommandOptions readCommandOptions(const CLICommand command, const int argc, char* argv[]);

        static void commandOptionError(const std::string& message);

        static void help();
        static int execute(const char* filePath);
        static int assemble(const char* filePath);
        static int compile(const char* filePath, const OptimisationLevel optimisationLevel);
        static int run(const char* filePath, const bool outputAssembly, const bool outputByteCode, const OptimisationLevel optimisationLevel);

        static void runVM(const std::vector<uint8_t>& bytecode);

        static std::string readSourceCode(const std::filesystem::path& path);

        static int outputBytecodeFile(const std::filesystem::path& outputFilePath, const std::vector<uint8_t>& bytecode);
        static int outputAssemblyFile(const std::filesystem::path &path, const std::vector<std::string>& assembly);

        static bool isValidFileExtension(const std::filesystem::path& filePath, const std::string_view* extension);

        static void handleUnsupportedFileExtensionError(const std::filesystem::path* path);
    };
}


#endif //SIMPLE_VM_DRIVER_H