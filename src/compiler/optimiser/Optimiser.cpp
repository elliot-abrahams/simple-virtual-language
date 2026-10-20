#include "Optimiser.h"

#include "../../Driver.h"

using namespace compiler;

Optimiser::Optimiser(const std::filesystem::path& filePath, const driver::OptimisationLevel optimisationLevel) :
    path(filePath), optimisationLevel(optimisationLevel) {}

void Optimiser::optimiseProgram(const ast::Program& program) {
    if (this->optimisationLevel == driver::OptimisationLevel::DEFAULT) {
        this->constantFolder->optimise(program);
    }
}
