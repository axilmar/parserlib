#ifndef TESTPL_PARSER_HPP
#define TESTPL_PARSER_HPP


#include "tokenizer.hpp"


//ast type enumeration
enum AST_TYPE {
    AST_TYPE_CHAR,
    AST_TYPE_INT,
    AST_TYPE_DOUBLE,
    AST_TYPE_NAME,
    AST_TYPE_PTR,
    AST_DECL_VARIABLE_NAME,
    AST_DECL_VARIABLE
};


//ast info
struct AST {
    AST_TYPE type;
    std::vector<Token> tokens;
    std::vector<AST> children;
};


//parse input into an ast
std::vector<AST> parse(const std::vector<Token>& tokens, std::vector<Error>& errors);


#endif //TESTPL_PARSER_HPP
