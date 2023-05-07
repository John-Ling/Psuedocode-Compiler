#ifndef Compiler_H
#define Compiler_H
#include <iostream>

#include "lexer.h"
#include "parser.h"
#include "semantics.h"
#include "code_generator.h"

class Compiler
{    
    private:
        std::string sourceFile;
    public: 
        Compiler(std::string filename);
        int compile(void);
};

#endif