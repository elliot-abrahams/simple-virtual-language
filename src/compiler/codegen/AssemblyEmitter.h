#ifndef SV_ASSEMBLYEMITTER_H
#define SV_ASSEMBLYEMITTER_H

#pragma once

#include <string>
#include <unordered_set>
#include <vector>
#include "Assembly.h"
#include "Builtins.h"


namespace compiler {
    class AssemblyEmitter {

    public:
        AssemblyEmitter();

        std::vector<std::string> emitProgram(const std::vector<AssemblyItem>& assemblyIR, const std::unordered_set<BuiltinFunctionId>& requiredBuiltinFunctions, const std::unordered_set<BuiltinDataId>& requiredBuiltinData);

    private:
        void emitDirective(const Directive directive, const std::unordered_set<BuiltinFunctionId>& requiredBuiltinFunctions);
        void emitInstruction(const Instruction instruction);

        std::string emitOpcode(const Opcode opcode) const;
        std::string emitOperand(const Operand operand) const;
        std::string emitNumber(const Number number) const;
        std::string emitNativeRef(const NativeRef nativeRef) const;
        std::string emitType(const AssemblyType assemblyType) const;

        std::string emitLabelDef(const LabelDef labelDef) const;
        void emitMethodDef(const MethodDef methodDef);
        void emitDataDef(const DataDef dataDef);

        void emitBuiltinFunctions(const std::unordered_set<BuiltinFunctionId>& requiredBuiltinFunctions);
        void emitBuiltinData(const std::unordered_set<BuiltinDataId>& requiredBuiltinData);

        void emit(const std::string& assemblyLine);
        void emitWithSingleIdent(const std::string& assemblyLine);
        void emitWithDoubleIndent(const std::string& assemblyLine);

        std::vector<std::string> assembly;
    };
}


#endif //SV_ASSEMBLYEMITTER_H