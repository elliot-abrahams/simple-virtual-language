#include "AssemblyEmitter.h"

#include <iomanip>
#include <ostream>

compiler::AssemblyEmitter::AssemblyEmitter(const std::string& filepath) {
    this->debugSources.push_back(
        DebugSourceInfo{
            0,
            filepath
        }
    );
    return;
}

std::vector<std::string> compiler::AssemblyEmitter::emitAssembly(const std::vector<AssemblyItem>& assemblyIR, const std::unordered_set<BuiltinFunctionId>& requiredBuiltinFunctions, const std::unordered_set<BuiltinDataId>& requiredBuiltinData) {
    this->emitProgram(assemblyIR, requiredBuiltinFunctions, requiredBuiltinData);
    this->emitDebugSection();
    return this->assembly;
}

void compiler::AssemblyEmitter::emitProgram(const std::vector<AssemblyItem> &assemblyIR, const std::unordered_set<BuiltinFunctionId> &requiredBuiltinFunctions, const std::unordered_set<BuiltinDataId> &requiredBuiltinData) {
    for (const auto& assemblyItem : assemblyIR) {

        // DIRECTIVE
        if (std::holds_alternative<Directive>(assemblyItem)) {
            this->emitDirective(std::get<Directive>(assemblyItem), requiredBuiltinFunctions);

            // INSTRUCTION
        } else if (std::holds_alternative<Instruction>(assemblyItem)) {
            this->emitInstruction(std::get<Instruction>(assemblyItem));

            // LABEL_DEF
        } else if (std::holds_alternative<LabelDef>(assemblyItem)) {
            this->emitWithSingleIdent(this->emitLabelDef(std::get<LabelDef>(assemblyItem)));

            // METHOD_DEF
        } else if (std::holds_alternative<MethodDef>(assemblyItem)) {
            this->emitMethodDef(std::get<MethodDef>(assemblyItem));

            // DATA_DEF
        } else if (std::holds_alternative<DataDef>(assemblyItem)) {
            this->emitDataDef(std::get<DataDef>(assemblyItem));
        }
    }

    this->emitBuiltinData(requiredBuiltinData);
}

void compiler::AssemblyEmitter::emitDirective(const Directive directive, const std::unordered_set<BuiltinFunctionId>& requiredBuiltinFunctions) {

    switch (directive) {
        case Directive::DATA:

            this->emitBuiltinFunctions(requiredBuiltinFunctions);

            this->emit(".data");
    }
}

void compiler::AssemblyEmitter::emitInstruction(const Instruction instruction) {
    const uint32_t startAddress = this->currentAddress;
    std::string assemblyLine = "";

    // OPCODE
    assemblyLine += emitOpcode(instruction.opcode);

    // OPERANDS
    for (const auto operand : instruction.operands) {
        assemblyLine += ' ';
        assemblyLine += this->emitOperand(operand);
    }

    this->emitWithDoubleIndent(assemblyLine);

    if (!instruction.source.has_value()) {
        return;
    }

    this->debugLines.push_back(DebugLineInfo{
        startAddress,
        this->currentAddress,
        0,
        instruction.source->line,
        instruction.source->column,
    });
}

std::string compiler::AssemblyEmitter::emitOpcode(const Opcode opcode) {
    this->currentAddress++;
    switch (opcode) {
        case Opcode::NOP: return"nop";
        case Opcode::HALT: return "halt";

        case Opcode::PUSH: return "push";
        case Opcode::POP: return "pop";
        case Opcode::DUP: return "dup";
        case Opcode::SWAP: return "swap";

        case Opcode::LOAD: return "load";
        case Opcode::LOADB: return "loadB";
        case Opcode::LOADG: return "loadG";
        case Opcode::LOADL: return "loadL";
        case Opcode::STORE: return "store";
        case Opcode::STOREB: return "storeB";
        case Opcode::STOREG: return "storeG";
        case Opcode::STOREL: return "storeL";
        case Opcode::ALLOC: return "alloc";
        case Opcode::FREE: return "free";

        case Opcode::NATIVE: return "native";
        case Opcode::CALL: return "call";
        case Opcode::RET: return "ret";
        case Opcode::JMP: return "jmp";
        case Opcode::JEZ: return "jez";
        case Opcode::JNZ: return "jnz";

        case Opcode::ADD: return "add";
        case Opcode::SUB: return "sub";
        case Opcode::MUL: return "mul";
        case Opcode::DIV: return "div";
        case Opcode::MOD: return "mod";
        case Opcode::NOT: return "not";
        case Opcode::AND: return "and";
        case Opcode::ORR: return "orr";
        case Opcode::XOR: return "xor";
        case Opcode::SHL: return "shl";
        case Opcode::SHR: return "shr";
        case Opcode::SAR: return "sar";
        case Opcode::CEQ: return "ceq";
        case Opcode::CNE: return "cne";
        case Opcode::CLT: return "clt";
        case Opcode::CLE: return "cle";
        case Opcode::CGT: return "cgt";
        case Opcode::CGE: return "cge";

        case Opcode::CONV: return "conv";
    }
}

std::string compiler::AssemblyEmitter::emitOperand(const Operand operand) {
    // ASSEMBLY TYPE
    if (std::holds_alternative<AssemblyType>(operand)) {
        return this->emitType(std::get<AssemblyType>(operand));
    }

    // IMMEDIATE
    if (std::holds_alternative<Immediate>(operand)) {
        return "#" + this->emitNumber(std::get<Immediate>(operand).value);
    }

    // LABEL_REF
    if (std::holds_alternative<LabelRef>(operand)) {
        this->currentAddress += 4;
        return "$" + std::get<LabelRef>(operand).name;
    }

    // NATIVE_REF
    if (std::holds_alternative<NativeRef>(operand)) {
        return this->emitNativeRef(std::get<NativeRef>(operand));
    }
}

std::string compiler::AssemblyEmitter::emitNumber(const Number number) {
    if (std::holds_alternative<int32_t>(number.value)) {
        this->currentAddress += 4;
        return std::to_string(std::get<int32_t>(number.value));
    }
    if (std::holds_alternative<uint32_t>(number.value)) {
        this->currentAddress += 4;
        return std::to_string(std::get<uint32_t>(number.value));
    }
    if (std::holds_alternative<int64_t>(number.value)) {
        this->currentAddress += 8;
        return std::to_string(std::get<int64_t>(number.value));
    }
    if (std::holds_alternative<uint64_t>(number.value)) {
        this->currentAddress += 8;
        return std::to_string(std::get<uint64_t>(number.value));
    }
    if (std::holds_alternative<float>(number.value)) {
        this->currentAddress += 4;
        return std::to_string(std::get<float>(number.value));
    }
    if (std::holds_alternative<double>(number.value)) {
        this->currentAddress += 8;
        return std::to_string(std::get<double>(number.value));
    }
}

std::string compiler::AssemblyEmitter::emitNativeRef(const NativeRef nativeRef) {
    this->currentAddress++;
    switch (nativeRef) {
        case NativeRef::EXIT: return "exit";
        case NativeRef::PRINT: return "print";
        case NativeRef::PRINT_STR: return "print_str";
    }
}

std::string compiler::AssemblyEmitter::emitType(const AssemblyType assemblyType) {
    this->currentAddress++;
    switch (assemblyType) {
        case AssemblyType::I32: return "i32";
        case AssemblyType::UI32: return "ui32";
        case AssemblyType::I64: return "i64";
        case AssemblyType::UI64: return "ui64";
        case AssemblyType::F32: return "f32";
        case AssemblyType::F64: return "f64";
        case AssemblyType::PTR: return "ptr";
        case AssemblyType::STR: return "str";
    }
}

std::string compiler::AssemblyEmitter::emitLabelDef(const LabelDef labelDef) {
    return "$" + labelDef.name + ":";
}

void compiler::AssemblyEmitter::emitMethodDef(const MethodDef methodDef) {
    const uint32_t startAddress = this->currentAddress;
    this->currentAddress += 5;
    this->emit("");
    this->emit("def " + this->emitLabelDef(methodDef.name));

    this->emitWithDoubleIndent("args " + std::to_string(methodDef.numberOfArguments));
    this->emitWithDoubleIndent("locals " + std::to_string(methodDef.numberOfLocals));
    this->emit("");

    this->debugLines.push_back(DebugLineInfo{
        startAddress,
        this->currentAddress,
        0,
        methodDef.source->line,
        methodDef.source->column,
    });
}

void compiler::AssemblyEmitter::emitDataDef(const DataDef dataDef) {
    std::string dataValue = "";

    if (std::holds_alternative<Number>(dataDef.value)) {
        dataValue = this->emitNumber(std::get<Number>(dataDef.value));
    } else if (std::holds_alternative<std::string>(dataDef.value)) {
        dataValue = "\"" + std::get<std::string>(dataDef.value) + "\"";
    }

    this->emitWithSingleIdent(this->emitLabelDef(dataDef.name) + " " + this->emitType(dataDef.type) + " " + dataValue);
}

void compiler::AssemblyEmitter::emitBuiltinFunctions(const std::unordered_set<BuiltinFunctionId>& requiredBuiltinFunctions) {
    for (const auto& builtinFunctionId : requiredBuiltinFunctions) {
        const BuiltinFunction* builtinFunction = Builtins::getBuiltinFunction(builtinFunctionId);

        // emit method def
        this->emit("");
        this->emit("def $" + builtinFunction->functionLabel + ":");

        // emit method metadata
        this->emitWithDoubleIndent("args " + std::to_string(builtinFunction->numberOfArguments));
        this->emitWithDoubleIndent("locals " + std::to_string(builtinFunction->numberOfLocals));
        this->emit("");

        // emit method body
        for (const auto& assembly : builtinFunction->functionBodyAssembly) {
            this->emitWithSingleIdent(assembly);
        }
        this->emit("");
    }
}

void compiler::AssemblyEmitter::emitBuiltinData(const std::unordered_set<BuiltinDataId> &requiredBuiltinData) {
    for (const auto& builtinDataId : requiredBuiltinData) {
        this->emitWithSingleIdent(*Builtins::getBuiltinData(builtinDataId));
    }
}

void compiler::AssemblyEmitter::emitDebugSection() {
    this->emit("");
    this->emit(".debug");

    this->emitDebugSourceSection();
    this->emitDebugLineTableSection();
}

void compiler::AssemblyEmitter::emitDebugSourceSection() {
    this->emit("");
    this->emitWithSingleIdent(".source");

    // for each source info entry
    for (const auto& sourceInfo : this->debugSources) {

        std::string sourceId = std::to_string(sourceInfo.sourceId);
        std::string spaces(5 - sourceId.length(), ' ');

        this->emitWithDoubleIndent(spaces + sourceId + "    \"" + sourceInfo.path + "\"");
    }
}

void compiler::AssemblyEmitter::emitDebugLineTableSection() {
    this->emit("");
    this->emitWithSingleIdent(".line_table");

    this->emitWithDoubleIndent(";     start          end       source       line       column");

    DebugLineInfo* previousLineInfo = &this->debugLines[0];
    bool mergeLastDebugLine = false;
    // for each line table entry
    for (uint32_t lineIdx = 1; lineIdx < this->debugLines.size(); ++lineIdx) {

        // while previous debugLine (source, line, column matches current debugLine
        // and previous end address is the same as the current start address
        while (
            lineIdx < this->debugLines.size() &&
            this->debugLines[lineIdx - 1].endAddress == this->debugLines[lineIdx].startAddress &&
            previousLineInfo->sourceId == this->debugLines[lineIdx].sourceId &&
            previousLineInfo->line == this->debugLines[lineIdx].line &&
            previousLineInfo->column == this->debugLines[lineIdx].column
        ) {
            lineIdx++;
        }

        if (lineIdx == this->debugLines.size()) {
            mergeLastDebugLine = true;
        }
        this->emitDebugLine(
            previousLineInfo->startAddress,
            this->debugLines[lineIdx - 1].endAddress,
            previousLineInfo->sourceId,
            previousLineInfo->line,
            previousLineInfo->column
        );
        previousLineInfo = &this->debugLines[lineIdx];
    }

    // return if last debug line in the list was already emitted
    if (mergeLastDebugLine) return;

    // emit last debug line in the list
    this->emitDebugLine(
        this->debugLines[this->debugLines.size() - 1].startAddress,
        this->debugLines[this->debugLines.size() - 1].endAddress,
        this->debugLines[this->debugLines.size() - 1].sourceId,
        this->debugLines[this->debugLines.size() - 1].line,
        this->debugLines[this->debugLines.size() - 1].column
    );
}

void compiler::AssemblyEmitter::emitDebugLine(
        const uint32_t startAddress,
        const uint32_t endAddress,
        const uint16_t sourceId,
        const uint32_t line,
        const uint16_t column
) {
    std::stringstream startAddressOutput;
    std::stringstream endAddressOutput;
    // format string to represent uint32_t as "0x00000000"
    startAddressOutput << "0x" << std::uppercase << std::hex << std::setw(8) << std::setfill('0') << startAddress;
    endAddressOutput << "0x" << std::uppercase << std::hex << std::setw(8) << std::setfill('0') << endAddress;

    std::string spaces(10, ' ');

    std::string sourceIdOutput = std::to_string(sourceId);
    std::string lineOutput = std::to_string(line);
    std::string columnOutput = std::to_string(column);

    this->emitWithDoubleIndent(
        startAddressOutput.str() + "    " +
        endAddressOutput.str() + "    " +
        spaces.substr(0, (5 - sourceIdOutput.length())) + sourceIdOutput + "    " +
        spaces.substr(0, (10 - lineOutput.length())) + lineOutput + "    " +
        spaces.substr(0, (5 - columnOutput.length())) + columnOutput
    );
}

void compiler::AssemblyEmitter::emit(const std::string& assemblyLine) {
    this->assembly.push_back(assemblyLine);
}

void compiler::AssemblyEmitter::emitWithSingleIdent(const std::string& assemblyLine) {
    this->assembly.push_back("    " + assemblyLine);
}

void compiler::AssemblyEmitter::emitWithDoubleIndent(const std::string& assemblyLine) {
    this->assembly.push_back("        " + assemblyLine);
}