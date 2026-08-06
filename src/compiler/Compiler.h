#ifndef SVM_COMPILER_H
#define SVM_COMPILER_H

#include <filesystem>
#include <vector>

#include "AST.h"


namespace compiler {
    class Compiler {

    public:
        Compiler();

        static std::vector<std::string> compile(const std::string& sourceCode, const std::filesystem::path& path);
        static void compile(const std::filesystem::path& path);
        static std::unique_ptr<ast::Program> testParsing(const std::string& sourceCode);
    };
}


#endif //SVM_COMPILER_H