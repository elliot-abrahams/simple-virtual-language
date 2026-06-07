#include "Driver.h"
#include "assembler/Assembler.h"
#include "vm/VM.h"

#include <iostream>
#include <filesystem>
#include <fstream>


int Driver::runCLI(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: no command provided.'\n";
        return 1;
    }

    std::string command = argv[1];

    if (command == "assemble") {
        if (argc < 3) {
            std::cerr << "Error: no file path provided'\n";
            return 1;
        }
        return assemble(argv[2]);
    }

    if (command == "execute") {
        if (argc < 3) {
            std::cerr << "Error: no file path provided'\n";
            return 1;
        }
        return execute(argv[2]);
    }

    std::cerr << "Unknown command: " << command << std::endl;

    return 1;
}

int Driver::assemble(const char* filePath) {
    { // validate file extension
        std::filesystem::path path(filePath);
        if (path.extension() != ASSEMBLY_FILE_EXTENSION) {
            handleUnsupportedFileExtensionError(&path);
            return 1;
        }
    }

    Assembler* assembler = new Assembler();

    // assemble bytecode assembly
    auto bytecode = assembler->assemble(std::string(filePath));

    delete assembler;

    if (!bytecode.has_value()) {
        return 1;
    }

    // replace .svma extension with .svmb
    std::string outputFilePath(filePath);
    outputFilePath = outputFilePath.substr(0, outputFilePath.find_last_of("."));
    outputFilePath += BYTECODE_FILE_EXTENSION;

    // generate output file from bytecode stored in vector
    std::ofstream outputFile(outputFilePath);
    if (!outputFile.is_open()) {
        std::cerr << "Error: could not create and open " << outputFilePath << std::endl;
        return 1;
    }

    for (size_t i = 0; i < bytecode.value().size(); ++i) {
        outputFile << std::hex
                  << std::uppercase
                  << std::setw(2)
                  << std::setfill('0')
                  << static_cast<int>(bytecode.value()[i]);

        if (i + 1 < bytecode.value().size())
            std::cout << " ";
    }

    std::cout << std::dec << "\n";
    outputFile.close();
    return 0;
}

int Driver::execute(const char* filePath) {
    { // validate file extension
        std::filesystem::path path(filePath);
        if (path.extension() != BYTECODE_FILE_EXTENSION) {
            handleUnsupportedFileExtensionError(&path);
            return 1;
        }
    }

    std::ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        std::cerr << "Error: could not open " << filePath << std::endl;
    }

    // read bytecode from file
    std::vector<uint8_t> bytecode(
        (std::istreambuf_iterator<char>(inputFile)),
        std::istreambuf_iterator<char>()
    );

    // execute bytecode
    VM* vm = new VM();
    vm->run(&bytecode);
    delete vm;

    return 0;
}

void Driver::handleUnsupportedFileExtensionError(const std::filesystem::path* path) {
    std::cerr << "File extension \'"<< path->extension() << "\' is not supported" << std::endl;
}
