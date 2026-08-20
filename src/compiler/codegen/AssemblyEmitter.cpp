#include "AssemblyEmitter.h"

compiler::AssemblyEmitter::AssemblyEmitter() {}

std::vector<std::string> compiler::AssemblyEmitter::emitProgram(const std::vector<AssemblyItem>& assemblyIR, const std::unordered_set<BuiltinFunctionId>& requiredBuiltinFunctions, const std::unordered_set<BuiltinDataId>& requiredBuiltinData) {
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

    return this->assembly;
}
void compiler::AssemblyEmitter::emitDirective(const Directive directive, const std::unordered_set<BuiltinFunctionId>& requiredBuiltinFunctions) {

    switch (directive) {
        case Directive::DATA:

            this->emitBuiltinFunctions(requiredBuiltinFunctions);

            this->emit(".data");
    }
}

void compiler::AssemblyEmitter::emitInstruction(const Instruction instruction) {
    std::string assemblyLine = "";

    // OPCODE
    assemblyLine += emitOpcode(instruction.opcode);

    // OPERANDS
    for (const auto operand : instruction.operands) {
        assemblyLine += ' ';
        assemblyLine += this->emitOperand(operand);
    }

    this->emitWithDoubleIndent(assemblyLine);
}

std::string compiler::AssemblyEmitter::emitOpcode(const Opcode opcode) const {
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

std::string compiler::AssemblyEmitter::emitOperand(const Operand operand) const {
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
        return "$" + std::get<LabelRef>(operand).name;
    }

    // NATIVE_REF
    if (std::holds_alternative<NativeRef>(operand)) {
        return this->emitNativeRef(std::get<NativeRef>(operand));
    }
}

std::string compiler::AssemblyEmitter::emitNumber(const Number number) const {
    if (std::holds_alternative<int32_t>(number.value)) {
        return std::to_string(std::get<int32_t>(number.value));
    }
    if (std::holds_alternative<uint32_t>(number.value)) {
        return std::to_string(std::get<uint32_t>(number.value));
    }
    if (std::holds_alternative<int64_t>(number.value)) {
        return std::to_string(std::get<int64_t>(number.value));
    }
    if (std::holds_alternative<uint64_t>(number.value)) {
        return std::to_string(std::get<uint64_t>(number.value));
    }
    if (std::holds_alternative<float>(number.value)) {
        return std::to_string(std::get<float>(number.value));
    }
    if (std::holds_alternative<double>(number.value)) {
        return std::to_string(std::get<double>(number.value));
    }
}

std::string compiler::AssemblyEmitter::emitNativeRef(const NativeRef nativeRef) const {
    switch (nativeRef) {
        case NativeRef::EXIT: return "exit";
        case NativeRef::PRINT: return "print";
        case NativeRef::PRINT_STR: return "print_str";
    }
}

std::string compiler::AssemblyEmitter::emitType(const AssemblyType assemblyType) const {
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

std::string compiler::AssemblyEmitter::emitLabelDef(const LabelDef labelDef) const {
    return "$" + labelDef.name + ":";
}

void compiler::AssemblyEmitter::emitMethodDef(const MethodDef methodDef) {
    this->emit("");
    this->emit("def " + this->emitLabelDef(methodDef.name));

    this->emitWithDoubleIndent("args " + std::to_string(methodDef.numberOfArguments));
    this->emitWithDoubleIndent("locals " + std::to_string(methodDef.numberOfLocals));
    this->emit("");
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

void compiler::AssemblyEmitter::emit(const std::string& assemblyLine) {
    this->assembly.push_back(assemblyLine);
}

void compiler::AssemblyEmitter::emitWithSingleIdent(const std::string& assemblyLine) {
    this->assembly.push_back("    " + assemblyLine);
}

void compiler::AssemblyEmitter::emitWithDoubleIndent(const std::string& assemblyLine) {
    this->assembly.push_back("        " + assemblyLine);
}