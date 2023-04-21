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
    TOKEN_KEYWORD_TYPEDEF,
    TOKEN_KEYWORD_DOUBLE,
    TOKEN_KEYWORD_STRUCT,
    TOKEN_KEYWORD_WRITE,
    TOKEN_KEYWORD_CHAR,
    TOKEN_KEYWORD_ENUM,
    TOKEN_KEYWORD_READ,
    TOKEN_KEYWORD_INT,
    TOKEN_KEYWORD_NEW,
    TOKEN_IDENTIFIER,
    TOKEN_LITERAL_INTEGER,
    TOKEN_LITERAL_FLOAT,
    TOKEN_LITERAL_STRING,
    TOKEN_LITERAL_CHAR,
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

    bool operator == (TOKEN_TYPE tokenType) const {
        return type == tokenType;
    }
};


//tokenize source
std::vector<Token> tokenize(const SourceType& input, std::vector<Error>& errors);


#endif //TESTPL_TOKENIZER_HPP
