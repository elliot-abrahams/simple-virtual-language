#ifndef SV_OPTIMISER_H
#define SV_OPTIMISER_H


#include <filesystem>

#include "ConstantFolder.h"
#include "../../Driver.h"

namespace compiler {

    class Optimiser {
    public:
        Optimiser(const std::filesystem::path& filePath, const driver::OptimisationLevel optimisationLevel);

        void optimiseProgram(const ast::Program& program);

    private:
        const std::filesystem::path& path;
        const driver::OptimisationLevel optimisationLevel;
        ConstantFolder* constantFolder = new ConstantFolder(this->path);
    };

}


#endif //SV_OPTIMISER_H