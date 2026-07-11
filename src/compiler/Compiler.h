#ifndef SVM_COMPILER_H
#define SVM_COMPILER_H
#include <filesystem>
#include <vector>


namespace compiler {
    class Compiler {

    public:
        Compiler();

        static void compile(const std::filesystem::path& path);

    private:
        static std::string readFile(const std::filesystem::path& path);

        static void generateFile(const std::filesystem::path& path, const std::vector<std::string>& assembly);
    };
}


#endif //SVM_COMPILER_H