#ifndef PARSERLIB__EXAMPLES__JAVA__TOKENIZER__HPP
#define PARSERLIB__EXAMPLES__JAVA__TOKENIZER__HPP


#include "character_parser.hpp"


namespace java
{


    enum class token_type
    {
        KEYWORD_ABSTRACT,
        KEYWORD_ASSERT,
        KEYWORD_BOOLEAN,
        KEYWORD_BREAK,
        KEYWORD_BYTE,
        KEYWORD_CASE,
        KEYWORD_CATCH,
        KEYWORD_CHAR,
        KEYWORD_CLASS,
        KEYWORD_CONST,
        KEYWORD_CONTINUE,
        KEYWORD_DEFAULT,
        KEYWORD_DO,
        KEYWORD_DOUBLE,
        KEYWORD_ELSE,
        KEYWORD_ENUM,
        KEYWORD_EXTENDS,
        KEYWORD_FINAL,
        KEYWORD_FINALLY,
        KEYWORD_FLOAT,
        KEYWORD_FOR,
        KEYWORD_IF,
        KEYWORD_GOTO,
        KEYWORD_IMPLEMENTS,
        KEYWORD_IMPORT,
        KEYWORD_INSTANCEOF,
        KEYWORD_INT,
        KEYWORD_INTERFACE,
        KEYWORD_LONG,
        KEYWORD_NATIVE,
        KEYWORD_NEW,
        KEYWORD_PACKAGE,
        KEYWORD_PRIVATE,
        KEYWORD_PROTECTED,
        KEYWORD_PUBLIC,
        KEYWORD_RETURN,
        KEYWORD_SHORT,
        KEYWORD_STATIC,
        KEYWORD_STRICTFP,
        KEYWORD_SUPER,
        KEYWORD_SWITCH,
        KEYWORD_SYNCHRONIZED,
        KEYWORD_THIS,
        KEYWORD_THROW,
        KEYWORD_THROWS,
        KEYWORD_TRANSIENT,
        KEYWORD_TRY,
        KEYWORD_VOID,
        KEYWORD_VOLATILE,
        KEYWORD_WHILE,

        IDENTIFIER,
        
        LITERAL_INTEGER,
        LITERAL_FLOAT,
        LITERAL_BOOLEAN,
        LITERAL_CHARACTER,
        LITERAL_STRING,
        LITERAL_NULL,

        SEPARATOR_LEFT_PARENTHESIS,
        SEPARATOR_RIGHT_PARENTHESIS,
        SEPARATOR_LEFT_CURLY_BRACKET,
        SEPARATOR_RIGHT_CURLY_BRACKET,
        SEPARATOR_LEFT_SQUARE_BRACKET,
        SEPARATOR_RIGHT_SQUARE_BRACKET,
        SEPARATOR_SEMICOLON,
        SEPARATOR_COMMA,
        SEPARATOR_DOT,

        OPERATOR_ASSIGN,
        OPERATOR_GREATER_THAN,
        OPERATOR_LESS_THAN,
        OPERATOR_LOGICAL_NOT,
        OPERATOR_BITWISE_NOT,
        OPERATOR_CONDITIONAL,
        OPERATOR_COLON,
        OPERATOR_EQUAL_TO,
        OPERATOR_LESS_THAN_OR_EQUAL_TO,
        OPERATOR_GREATER_THAN_OR_EQUAL_TO,
        OPERATOR_DIFFERENT_THAN,
        OPERATOR_LOGICAL_AND,
        OPERATOR_LOGICAL_OR,
        OPERATOR_INCREMENT,
        OPERATOR_DECREMENT,
        OPERATOR_ADD,
        OPERATOR_SUBTRACT,
        OPERATOR_MULTIPLY,
        OPERATOR_DIVIDE,
        OPERATOR_BITWISE_AND,
        OPERATOR_BITWISE_OR,
        OPERATOR_BITWISE_XOR,
        OPERATOR_REMAINDER,
        OPERATOR_LEFT_SHIFT,
        OPERATOR_RIGHT_SHIFT,
        OPERATOR_SIGNED_RIGHT_SHIFT,
        OPERATOR_ASSIGN_ADD,
        OPERATOR_ASSIGN_SUBTRACT,
        OPERATOR_ASSIGN_MULTIPLY,
        OPERATOR_ASSIGN_DIVIDE,
        OPERATOR_ASSIGN_BITWISE_AND,
        OPERATOR_ASSIGN_BITWISE_OR,
        OPERATOR_ASSIGN_BITWISE_XOR,
        OPERATOR_ASSIGN_LEFT_SHIFT,
        OPERATOR_ASSIGN_RIGHT_SHIFT,
        OPERATOR_ASSIGN_SIGNED_RIGHT_SHIFT
    };


    //token
    struct token
    {
        token_type type;
        std::basic_string_view<char16_t> begin;
        std::basic_string_view<char16_t> end;
    };


    //tokenize input
    std::vector<token> tokenize(const java_string& input, std::vector<Error>& errors);


} //namespace java


#endif //PARSERLIB__EXAMPLES__JAVA__TOKENIZER__HPP
