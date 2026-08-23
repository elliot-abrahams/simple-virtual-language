#ifndef SV_ASSEMBLYEMITTER_H
#define SV_ASSEMBLYEMITTER_H

#pragma once

#include <string>
#include <unordered_set>
#include <vector>
#include "Assembly.h"
#include "Builtins.h"


namespace compiler {

    struct DebugSourceInfo {
        uint16_t sourceId;
        std::string path;
    };

    struct DebugFunctionInfo {
        std::string name;
        uint32_t startAddress;
        uint32_t endAddress;
    };

    struct DebugLineInfo {
        uint32_t startAddress;
        uint32_t endAddress;
        uint16_t sourceId;
        uint32_t line;
        uint16_t column;
    };

    class AssemblyEmitter {
    public:
        AssemblyEmitter(const std::string& filepath);

        std::vector<std::string> emitAssembly(const std::vector<AssemblyItem>& assemblyIR, const std::unordered_set<BuiltinFunctionId>& requiredBuiltinFunctions, const std::unordered_set<BuiltinDataId>& requiredBuiltinData);

    private:
        void emitProgram(const std::vector<AssemblyItem>& assemblyIR, const std::unordered_set<BuiltinFunctionId>& requiredBuiltinFunctions, const std::unordered_set<BuiltinDataId>& requiredBuiltinData);

        void emitDirective(const Directive directive, const std::unordered_set<BuiltinFunctionId>& requiredBuiltinFunctions);
        void emitInstruction(const Instruction &instruction);

        std::string emitOpcode(const Opcode opcode);
        std::string emitOperand(const Operand &operand);
        std::string emitNumber(const Number number);
        std::string emitNativeRef(const NativeRef nativeRef);
        std::string emitType(const AssemblyType assemblyType);

        static std::string emitLabelDef(const LabelDef &labelDef);
        void emitMethodDef(const MethodDef &methodDef);
        void emitDataDef(const DataDef &dataDef);

        void emitBuiltinFunctions(const std::unordered_set<BuiltinFunctionId>& requiredBuiltinFunctions);
        void emitBuiltinData(const std::unordered_set<BuiltinDataId>& requiredBuiltinData);

        void emitDebugSection();
        void emitDebugSourceSection();
        void emitDebugFunctions();
        void emitDebugLineTableSection();
        void emitDebugLine(const uint32_t startAddress, const uint32_t endAddress, const uint16_t sourceId, const uint32_t line, const uint16_t column);

        void handleIRMarker(const IRMarker marker);

        void emit(const std::string& assemblyLine);
        void emitWithSingleIdent(const std::string& assemblyLine);
        void emitWithDoubleIndent(const std::string& assemblyLine);

        std::vector<std::string> assembly;

        uint32_t currentAddress = 0;
        std::vector<DebugSourceInfo> debugSources;
        std::vector<DebugFunctionInfo> debugFunctions;
        std::vector<DebugLineInfo> debugLines;

        uint32_t startAddressOfCurrentMethodDecl = 0;
        std::optional<const MethodDef*> currentMethodDef = std::nullopt;
    };
}


#endif //SV_ASSEMBLYEMITTER_H