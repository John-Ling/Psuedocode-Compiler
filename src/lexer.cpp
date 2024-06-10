#include "lexer.hpp"

Lexer::Lexer(std::string filename)
{
    this->sourceFile = filename;
    this->position = -1; // index for characters in each line of file
}

std::vector<std::string> Lexer::get_source_file_lines(void)
{
    return this->sourceFileLines;
}

int Lexer::generate_tokens(void)
{
    // open file 
    std::ifstream fileReader(sourceFile);
    if (!fileReader.is_open())
    {
        return 1;
    }
    
    while (fileReader)
    {
        std::string line;
        std::getline(fileReader, line);
        this->sourceFileLines.push_back(line);
        if (line != "")
        {
            try
            {
                tokenize_line(line);
            }
            catch(Error &e)
            {
                e.display_problem();
                exit(1);
            }
        }
    }
    // add final EOF token
    Token token(Tokens::END_OF_FILE, Keywords::END_OF_FILE);
    this->tokens.push_back(token);
    return 0;
}

void Lexer::tokenize_line(std::string line)
{
    const std::string WHITESPACE = " ";
    this->currentLine = line;
    advance();
    
    while (this->position != -1) // read until end of line
    {
        std::string character = {this->currentLine[this->position]};
        if (this->KEYWORDS_TO_TOKENS.count(character)) // common single character tokens such as brackets, commas, colons etc
        {
            Token token = lookahead(character);
            this->tokens.push_back(token);
        }
        else if (is_letter(character)) // build a keyword or identifier from alphanumeric characters
        {
            Token token = get_keyword_or_identifier();
            this->tokens.push_back(token);
        }
        else if (is_integer(character))
        {
            Token token = get_numerical_literal();
            this->tokens.push_back(token);
        }
        else if (character == "\"")
        {
            Token token = get_string_literal();
            this->tokens.push_back(token);
        }
        advance();
    }
    Token endOfLineToken;
    endOfLineToken.type = Tokens::END_OF_LINE;
    endOfLineToken.value = "";
    this->tokens.push_back(endOfLineToken);
}

// "peeks" at the next two character to take the valid maximal munch of a potential token
// when given two token choices < or or <= or <--, <-- will be chosen as its the longest valid token
Token Lexer::lookahead(std::string character)
{
    if (character == Keywords::LESSER || character == Keywords::GREATER) // in psuedocode there are only two characters with double or triple operators
    {
        const unsigned int LONGEST_OPERATOR_LENGTH = 3; // the assignment operator <-- is 3 characters long
        unsigned int offset = 0; // determines how many characters the function skips over when completed as those characters combine into a single token
        std::string largestOperator = {character};
        Token token(this->KEYWORDS_TO_TOKENS.at(character), character);

        for (unsigned int i = 1; i < LONGEST_OPERATOR_LENGTH; i++) // look two steps ahead to match double or triple letter operators
        {
            if (this->position + i < this->currentLine.length())
            {
                char nextCharacter = this->currentLine[this->position + i];
                largestOperator = largestOperator + nextCharacter;
                if (this->KEYWORDS_TO_TOKENS.count(largestOperator))
                {
                    offset = i;
                    token.type = this->KEYWORDS_TO_TOKENS.at(largestOperator); // generate token
                    token.value = largestOperator;
                }
            }
        }
        this->position += offset;
        return token;
    }
    else
    {
        Token token(this->KEYWORDS_TO_TOKENS.at(character), character);
        return token;
    }
}

void Lexer::advance()
{
    this->position++;
    if (this->position == -1 || this->position >= (int)this->currentLine.length()) // check if reached end of line
    {
        this->position = -1;
    }
    return;
}

bool Lexer::is_valid_identifier(std::string value)
{
    const char WHITESPACE = ' ';
    
    if (is_integer({value[0]}))
    {
        return false;
    }
    for (int i = 0; i < (int)value.length(); i++)
    {
        if (!is_alphanumeric({value[i]}) || value[i] == WHITESPACE)
        {
            return false;
        }
    }
    return true;
}

Token Lexer::get_keyword_or_identifier(void)
{
    Token token = Token();
    std::string buffer = {this->currentLine[this->position]};
    advance();
    while (this->position != -1 && is_alphanumeric({this->currentLine[this->position]}))
    {
        buffer = buffer + this->currentLine[this->position];
        advance();
    }
    
    if (this->KEYWORDS_TO_TOKENS.count(buffer))
    {
        token.type = this->KEYWORDS_TO_TOKENS.at(buffer);
    }
    else if (buffer == Keywords::TRUE || buffer == Keywords::FALSE)
    {
        token.type = Tokens::BOOLEAN_LITERAL;
    }
    else if (is_valid_identifier(buffer))
    {
        token.type = Tokens::IDENTIFIER;
    }
    else
    {
        token.type = Tokens::UNKNOWN;
    }
    token.value = buffer;
    this->position--;
    return token;
}

Token Lexer::get_numerical_literal(void)
{
    // get either a float or integer literal depending on the presence of a decimal point
    std::string type = Tokens::INTEGER_LITERAL;
    std::string number;
    while (this->position != -1)
    {
        std::string character = {this->currentLine[this->position]};
        if (!(is_integer(character) || character == ".")) 
        {
            break;
        }

        if (character == ".")
        {
            type = Tokens::FLOAT_LITERAL;
        }
        number = number + character;
        advance();
    }
    this->position--;
    Token token(type, number);
    return token;
}

Token Lexer::get_string_literal(void)
{
    // read up to ending character ) and create a string literal
    char character = this->currentLine[this->position];
    std::string stringLiteral;
    stringLiteral = stringLiteral + character; // add initial quotation mark
    advance(); 
    character = this->currentLine[this->position];
    while (character != '"') // read until next quotation mark
    {
        stringLiteral = stringLiteral + character;
        advance();
        character = this->currentLine[this->position];
    }
    stringLiteral = stringLiteral + character; // add closing quotation mark
    Token token(Tokens::STRING_LITERAL, stringLiteral);
    return token;
}

bool Lexer::is_integer(std::string character)
{
    char convertedCharacter = character[0];
    const int ASCII_0 = 48;
    const int ASCII_9 = 57;
    int ascii = (int)convertedCharacter;
    if (ascii >= ASCII_0 && ascii <= ASCII_9)
    {
        return true;
    }
    return false;
}

bool Lexer::is_letter(std::string character)
{
    char convertedCharacter = character[0];
    const int ASCII_MIN_LOWER = 97;
    const int ASCII_MIN_UPPER = 65;
    const int ASCII_MAX_LOWER = 122;
    const int ASCII_MAX_UPPER = 90;
    int ascii = (int)convertedCharacter;

    if ((ascii >= ASCII_MIN_LOWER && ascii <= ASCII_MAX_LOWER) || (ascii >= ASCII_MIN_UPPER && ascii <= ASCII_MAX_UPPER))
    {
        return true;
    }
    return false;
}

bool Lexer::is_alphanumeric(std::string character)
{
    // checks if letter is character from a-z or A-Z or 0-9
    char convertedCharacter = character[0];
    const int ASCII_MIN_LOWER = 97;
    const int ASCII_MIN_UPPER = 65;
    const int ASCII_MAX_LOWER = 122;
    const int ASCII_MAX_UPPER = 90;
    const int ASCII_0 = 48;
    const int ASCII_9 = 57;

    int ascii = (int)convertedCharacter;
    if ((ascii >= ASCII_MIN_LOWER && ascii <= ASCII_MAX_LOWER) || (ascii >= ASCII_MIN_UPPER && ascii <= ASCII_MAX_UPPER) || (ascii >= ASCII_0 && ascii <= ASCII_9))
    {
        return true;
    }
    return false;
}