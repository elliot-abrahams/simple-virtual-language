#ifndef SVM_ERROR_H
#define SVM_ERROR_H
#include <stdexcept>
#include <string>

struct CompilerError : std::runtime_error {
    std::string file;
    size_t line;
    size_t column;

    CompilerError(const std::string& file, const size_t line, const size_t column, const std::string& errorType, const std::string& message) :
          std::runtime_error(format(file, line, column, errorType, message)),
          file(std::move(file)),
          line(line),
          column(column) {}

private:
    static std::string format(const std::string& file, const size_t line, const size_t column, const std::string& errorType, const std::string& msg) {
        return file + ":" + std::to_string(line) + ":" +
               std::to_string(column) + ": " + errorType + ": " + msg;
    }
};

struct LexicalError final : CompilerError {
    LexicalError(const std::string& file, const size_t line, const size_t column, const std::string& message) :
          CompilerError(file, line, column, "SyntaxError", message) {}
};

struct SyntaxError final : CompilerError {
    SyntaxError(const std::string& file, const size_t line, const size_t column, const std::string& message) :
          CompilerError(file, line, column, "SyntaxError", message) {}
};

struct TypeError final : CompilerError {
    TypeError(const std::string& file, const size_t line, const size_t column, const std::string& message) :
          CompilerError(file, line, column, "TypeError", message) {}
};

struct SemanticError final : CompilerError {
    SemanticError(const std::string& file, const size_t line, const size_t column, const std::string& message) :
          CompilerError(file, line, column, "SemanticError", message) {}
};


#endif //SVM_ERROR_H