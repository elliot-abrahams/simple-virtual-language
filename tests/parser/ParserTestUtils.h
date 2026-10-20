#ifndef SVM_PARSERTESTUTILS_H
#define SVM_PARSERTESTUTILS_H

#include "../ASTTestUtils.h"
#include "../../src/include/Error.h"

using namespace testUtils;

namespace parserTest {

    inline std::unique_ptr<ast::Program> PARSE(
        const std::string& sourceCode
    ) {
        // parse source code
        return compiler::Compiler::testParsing(sourceCode);
    }
};

#endif //SVM_PARSERTESTUTILS_H