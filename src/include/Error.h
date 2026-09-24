#ifndef SVM_ERROR_H
#define SVM_ERROR_H
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <string>

enum class ErrorContext {
    LANGUAGE,
    INTERNAL
};

enum class RuntimeErrorType {
    EXPLICIT_ARRAY_INDEX_OUT_OF_RANGE,
    EXPLICIT_NEGATIVE_ARRAY_LENGTH,
    EXPLICIT_ARRAY_INITIALISER_LENGTH,

    DIVISION_BY_ZERO,
    OUT_OF_RANGE,
    STACK_OVERFLOW,
    OUT_OF_MEMORY,

    INTERNAL
};

struct SVLError : std::runtime_error {
    const std::string message;

    SVLError(const std::string& message) : std::runtime_error("Unknown"), message(message) {}

protected:
    std::string generateMessage(const std::filesystem::path& path, const uint32_t line, const uint16_t column, const std::string& errorType) const {
        std::string errorMessage;

        const std::string absPath = std::filesystem::absolute(path).string();
        std::ifstream sourceFile(path);

        // read line in source code
        std::string sourceLine;
        if (sourceFile) {
            for (size_t i = 1; i <= line; ++i) {
                if (!std::getline(sourceFile, sourceLine)) {
                    sourceLine.clear();
                    break;
                }
            }
        }

        // remove leading spaces from sourceLine
        uint32_t leadingWhitespace = 0;
        if (!sourceLine.empty()) {
            const auto pos = sourceLine.find_first_not_of(' ');
            if (pos != std::string::npos) {
                sourceLine.erase(0, pos);
                leadingWhitespace = pos;
            } else {
                sourceLine = " ";
            }
        }

        const std::string spaces(column + 4, ' ');
        const std::string lineNumber = std::to_string(line);

        errorMessage += errorType + ": " + this->message + "\n";

        if (!sourceLine.empty()) {
            errorMessage += "  " + lineNumber + " |    " + sourceLine + "\n";
            errorMessage += spaces.substr(0, (lineNumber.size() + 3)) + "|" + spaces.substr(0, 3 + (column - leadingWhitespace)) + "^" + "\n";

        } else { // unable to read source file
            errorMessage += "    | source code unavailable";
        }
        return errorMessage;
    }
};

struct RuntimeError final : SVLError {
    const RuntimeErrorType type;

    RuntimeError(const RuntimeErrorType type, const std::string& message) :
        SVLError(message), type(type) {}

    std::string generateMessage(const std::filesystem::path& path, const uint32_t line, const uint16_t column) const {
        std::string errorType;
        switch (this->type) {
            case RuntimeErrorType::EXPLICIT_ARRAY_INDEX_OUT_OF_RANGE: errorType =  "ArrayIndexOutOfRangeError"; break;
            case RuntimeErrorType::EXPLICIT_NEGATIVE_ARRAY_LENGTH: errorType = "NegativeArrayLengthError"; break;
            case RuntimeErrorType::EXPLICIT_ARRAY_INITIALISER_LENGTH: errorType =  "ArrayInitialiserLengthError"; break;
            case RuntimeErrorType::DIVISION_BY_ZERO: errorType = "DivisionByZeroError"; break;
            case RuntimeErrorType::OUT_OF_RANGE: errorType = "OutOfRangeError"; break;
            case RuntimeErrorType::STACK_OVERFLOW: errorType = "StackOverflowError"; break;
            case RuntimeErrorType::OUT_OF_MEMORY: errorType = "OutOfMemoryError"; break;
            case RuntimeErrorType::INTERNAL: errorType = "InternalError"; break;
        }
        return SVLError::generateMessage(path, line, column, errorType);
    }
};

struct SourceError : SVLError {
    const std::filesystem::path filepath;
    const uint32_t line;
    const uint16_t column;

    SourceError(const std::filesystem::path& filepath, const uint32_t line, const uint16_t column, const std::string& message) :
        SVLError(message), filepath(filepath), line(line), column(column) {}

    virtual std::string generateMessage() const = 0;

protected:
    std::string generateMessage(const std::string& errorType) const {
        std::string message = SVLError::generateMessage(this->filepath, this->line, this->column, errorType);
        message += "  at " + std::filesystem::absolute(this->filepath).string() + ":" + std::to_string(line) + ":" + std::to_string(column) + "\n";

        return message;
    }
};

struct AssemblerError : SourceError {
    AssemblerError(const std::filesystem::path& path, const uint32_t line, const uint16_t column, const std::string& message) :
        SourceError(path, line, column, message) {}

    std::string generateMessage() const override {
        return SourceError::generateMessage("AssemblerError");
    }
};

struct SyntaxError final : SourceError {
    SyntaxError(const std::filesystem::path& path, const uint32_t line, const uint16_t column, const std::string& message) :
        SourceError(path, line, column, message) {}

    std::string generateMessage() const override {
        return SourceError::generateMessage("SyntaxError");
    }
};

struct SemanticError final : SourceError {
    SemanticError(const std::filesystem::path& path, const uint32_t line, const uint16_t column, const std::string& message) :
        SourceError(path, line, column, message) {}

    std::string generateMessage() const override {
        return SourceError::generateMessage("SemanticError");
    }
};

struct TypeError final : SourceError {
    TypeError(const std::filesystem::path& path, const uint32_t line, const uint16_t column, const std::string& message) :
        SourceError(path, line, column, message) {}

    std::string generateMessage() const override {
        return SourceError::generateMessage("TypeError");
    }
};


#endif //SVM_ERROR_H