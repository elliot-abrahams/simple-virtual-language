#include "Driver.h"
#include "assembler/Assembler.h"
#include "vm/VM.h"

#include <iostream>
#include <filesystem>
#include <fstream>

#include "compiler/Compiler.h"


int Driver::runCLI(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: no command provided.'\n";
        return 1;
    }

    const std::string command = argv[1];

    if (command == "help") {
        if (argc > 3) {
            std::cerr << "Error: too many arguments.'\n";
            return 1;
        }
        help();
        return 0;
    }

    if (command == "run") {
        if (argc < 3) {
            std::cerr << "Error: no file path provided'\n";
            return 1;
        }
        if (argc > 5) {
            std::cerr << "Error: too many arguments.'\n";
            return 1;
        }

        bool outputAssembly = false;
        bool outputByteCode = false;
        for (int i = 2; i < argc - 1; i++) {
            std::string arg = argv[i];
            if (arg == "-a") outputAssembly = true;
            else if (arg == "-b") outputByteCode = true;
            else {
                std::cerr << "Error: unknown argument '" << arg << "'\n";
                return 1;
            }
        }

        return run(argv[argc - 1], outputAssembly, outputByteCode);
    }

    if (command == "compile") {
        if (argc < 3) {
            std::cerr << "Error: no file path provided'\n";
            return 1;
        }
        if (argc > 3) {
            std::cerr << "Error: too many arguments.'\n";
            return 1;
        }
        return compile(argv[2]);
    }

    if (command == "assemble") {
        if (argc < 3) {
            std::cerr << "Error: no file path provided'\n";
            return 1;
        }
        if (argc > 3) {
            std::cerr << "Error: too many arguments.'\n";
            return 1;
        }
        return assemble(argv[2]);
    }

    if (command == "execute") {
        if (argc < 3) {
            std::cerr << "Error: no file path provided'\n";
            return 1;
        }
        if (argc > 3) {
            std::cerr << "Error: too many arguments.'\n";
            return 1;
        }
        return execute(argv[2]);
    }

    std::cerr << "Unknown command: " << command << std::endl;

    return 1;
}

void Driver::help() {
    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "    help                       Display this help message" << std::endl;
    std::cout << "    execute <file.svmb>        Execute bytecode" << std::endl;
    std::cout << "    assemble <file.svma>       Assemble assembly into bytecode" << std::endl;
    std::cout << "    compile <file.sv>          Compile source code into assembly" << std::endl;
    std::cout << "    run [-a] [-b] <file.sv>    Compile, assemble and execute .sv source code" << std::endl;
    std::cout << std::endl;
    std::cout << "Run options:" << std::endl;
    std::cout << "    -a                         Output assembly in .svma file" << std::endl;
    std::cout << "    -b                         Output bytecode in .svmb file" << std::endl;
    std::cout << std::endl;
}

int Driver::execute(const char* filePath) {
    if (!isValidFileExtension(filePath, &BYTECODE_FILE_EXTENSION)) {
        return 1;
    }

    std::ifstream inputFile(filePath, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Error: could not open " << filePath << std::endl;
        return 1;
    }

    // read bytecode from file
    const std::vector<uint8_t> bytecode(
        (std::istreambuf_iterator<char>(inputFile)),
        std::istreambuf_iterator<char>()
    );

    // run bytecode in vm
    try {
        const auto vm = new VM();
        vm->run(&bytecode);
        exit(vm->getExitStatus());
    } catch (const VMError& e) {
        std::cerr << "VMError: " << e.what() << std::endl;
        return 1;
    }
}

int Driver::assemble(const char* filePath) {
    if (!isValidFileExtension(filePath, &ASSEMBLY_FILE_EXTENSION)) {
        return 1;
    }

    const auto assembler = new assembler::Assembler();

    // assemble bytecode assembly
    const auto bytecode = assembler->assemble(std::string(filePath));

    delete assembler;

    if (!bytecode.has_value()) {
        return 1;
    }

    // replace .svma extension with .svmb
    std::filesystem::path outputFilePath(filePath);
    outputFilePath.replace_extension(BYTECODE_FILE_EXTENSION);

    return outputBytecodeFile(outputFilePath, bytecode.value());
}

int Driver::compile(const char* filePath) {
    if (!isValidFileExtension(filePath, &PROGRAMMING_LANGUAGE_FILE_EXTENSION)) {
        return 1;
    }

    std::filesystem::path outputFilePath(filePath);

    const auto* compiler = new compiler::Compiler();
    const auto assembly = compiler->compile(readSourceCode(filePath), filePath);
    delete compiler;

    // replace .sv extension with .svma
    outputFilePath.replace_extension(ASSEMBLY_FILE_EXTENSION);

    return outputAssemblyFile(outputFilePath, assembly);
}

int Driver::run(const char *filePath, const bool outputAssembly, const bool outputByteCode) {
    if (!isValidFileExtension(filePath, &PROGRAMMING_LANGUAGE_FILE_EXTENSION)) {
        return 1;
    }

    // replace .sv extension with .svma
    std::filesystem::path outputFilePath(filePath);

    // compile code
    const auto* compiler = new compiler::Compiler();
    const auto assembly = compiler->compile(readSourceCode(filePath), filePath);
    delete compiler;

    outputFilePath.replace_extension(ASSEMBLY_FILE_EXTENSION);

    if (outputAssembly) {
        // generate .svma file
        const int exitCode = outputAssemblyFile(outputFilePath, assembly);
        if (exitCode != 0) {
            return exitCode;
        }
    }

    std::string assemblyLanguage;
    for (const auto& line : assembly) {
        assemblyLanguage += line + "\n";
    }

    // assemble bytecode assembly
    const auto assembler = new assembler::Assembler();
    const auto bytecode = assembler->assembleString(assemblyLanguage);
    delete assembler;
    if (!bytecode.has_value()) {
        return 1;
    }

    outputFilePath.replace_extension(BYTECODE_FILE_EXTENSION);

    if (outputByteCode) {
        // generate .svmb file
        const int exitCode = outputBytecodeFile(outputFilePath, bytecode.value());
        if (exitCode != 0) {
            return exitCode;
        }
    }

    // run bytecode in vm
    try {
        const auto vm = new VM();
        vm->run(&bytecode.value());
        exit(vm->getExitStatus());
    } catch (const VMError& e) {
        std::cerr << "VMError: " << e.what() << std::endl;
        return 1;
    }
}

std::string Driver::readSourceCode(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);

    if (!file) {
        std::cerr << "Error: could not open " << path << std::endl;
        exit(EXIT_FAILURE);
    }

    return {
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    };
}

int Driver::outputBytecodeFile(const std::filesystem::path& outputFilePath, const std::vector<uint8_t>& bytecode) {
    // generate output file from bytecode stored in vector
    std::ofstream outputFile(outputFilePath, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Error: could not create and open " << outputFilePath << std::endl;
        return 1;
    }

    outputFile.write(
        reinterpret_cast<const char*>(bytecode.data()),
        static_cast<std::streamsize>(bytecode.size())
    );

    outputFile << std::dec << "\n";
    outputFile.close();
    return 0;
}

int Driver::outputAssemblyFile(const std::filesystem::path& path, const std::vector<std::string>& assembly) {
    std::ofstream outputFile(path);

    if (!outputFile.is_open()) {
        std::cerr << "Error: could not create and open " << path << std::endl;
        return 1;
    }

    for (const auto& line : assembly) {
        outputFile << line << '\n';
    }
    return 0;
}

bool Driver::isValidFileExtension(const std::filesystem::path &filePath, const std::string_view* extension) {
    // validate file extension
    const std::filesystem::path path(filePath);
    if (path.extension() != *extension) {
        handleUnsupportedFileExtensionError(&path);
        return false;
    }
    return true;
}

void Driver::handleUnsupportedFileExtensionError(const std::filesystem::path* path) {
    std::cerr << "File extension \'"<< path->extension() << "\' is not supported" << std::endl;
}
