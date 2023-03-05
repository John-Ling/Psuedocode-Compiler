#ifndef Parser_H
#define Parser_H

#include <vector>
#include <iostream>
#include <stdexcept>

#include "token.h"
#include "constants.h"

class Parser
{
    private:
        std::vector<Token> tokens;
        std::vector<Token> currentLine;
        int pointer;
		bool errorOccurred;
        void parse_line(int pointer);
        int statement(void);
        bool match(std::string tokenType);
		void advance(void);
        int function(void);
		int function_parameter(void);
        int output(void);
        int input(void);
        int expression(void);
		int equality(void);
		int comparison(void);
		int term(void);
		int factor(void);
		int unary(void);
		int primary();
        int function_call_parameter(void);
        int primitive_literal(void);
		int primitive_type(void);
		int variable_declaration(void);
    public:
        Parser(std::vector<Token> tokens);
        int parse_tokens(void);
		int parser_test(void);
};

#endif