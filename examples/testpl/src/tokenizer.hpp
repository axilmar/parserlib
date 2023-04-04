#ifndef TESTPL_TOKENIZER_HPP
#define TESTPL_TOKENIZER_HPP


#include <string>
#include <vector>
#include "Error.hpp"


//char type is char
using Char = char;


//source type is basic_string<Char>
using SourceType = std::basic_string<Char>;


//token type
enum TOKEN_TYPE {
    TOKEN_TYPEDEF,
    TOKEN_STRUCT,
    TOKEN_WRITE,
    TOKEN_READ,
    TOKEN_NEW,
    TOKEN_IDENTIFIER,
    TOKEN_INTEGER,
    TOKEN_FLOAT,
    TOKEN_STRING,
    TOKEN_CHAR,
    TOKEN_TILDE,
    TOKEN_EXCLAMATION_MARK,
    TOKEN_PERCENT,
    TOKEN_AMBERSAND,
    TOKEN_STAR,
    TOKEN_OPENING_PARENTHESIS,
    TOKEN_CLOSING_PARENTHESIS,
    TOKEN_UNDERSCORE,
    TOKEN_MINUS,
    TOKEN_PLUS,
    TOKEN_EQUALS,
    TOKEN_OPENING_CURLY_BRACKET,
    TOKEN_CLOSING_CURLY_BRACKET,
    TOKEN_OPENING_SQUARE_BRACKET,
    TOKEN_CLOSING_SQUARE_BRACKET,
    TOKEN_PIPE,
    TOKEN_SEMICOLON,
    TOKEN_LESS_THAN,
    TOKEN_GREATER_THAN,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_QUESTION_MARK,
    TOKEN_DIV
};


//token type
struct Token {
    TOKEN_TYPE type;
    SourceType value;
    Position position;
};


//tokenize source
std::vector<Token> tokenize(const SourceType& input, std::vector<Error>& errors);


#endif //TESTPL_TOKENIZER_HPP
