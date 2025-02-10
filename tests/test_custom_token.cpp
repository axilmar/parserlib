#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <cassert>
#include "parserlib.hpp"


using namespace parserlib;


enum class TokenKind {
    A,
    B,
    Equal,
    Semicolumn
};


struct Token {
    TokenKind kind;
    std::string lexeme;
    int row;
    int column;

    Token(TokenKind k, const std::string& l, int r, int c) 
        : kind(k), lexeme(l), row(r), column(c)
    {
    }

    //    bool operator == (TokenKind tk) const {
    //        return kind == tk;
    //    }
};


// Global comparison operator
bool operator == (const Token& token, TokenKind tk) {
    return token.kind == tk;
}


enum class AST {
    Assignment,
    Declaration
};


static std::string to_string(const AST& ast) {
    switch (ast) {
    case AST::Assignment:
        return "Assignment";
    case AST::Declaration:
        return "Declaration";
        // Add other cases as needed
    default:
        return "Unknown AST type";
    }
}


static std::string to_string(const Token& token) {
    switch (token.kind) {
    case TokenKind::A:
        return "A";
    case TokenKind::B:
        return "B";
    case TokenKind::Equal:
        return "=";
    case TokenKind::Semicolumn:
        return ";";
    default:
        return "Unknown AST type";
    }
}


static std::ostream& operator << (std::ostream& os, const AST& ast) {
    os << to_string(ast);
    return os;
}


static std::ostream& operator << (std::ostream& os, const Token& token) {
    os << to_string(token);
    return os;
}


static std::ostream& operator << (std::ostream& os, const std::vector<Token>& tokens) {
    for (const Token& token : tokens) {
        os << to_string(token) << ' ';
    }
    return os;
}


using pe = parser_engine<std::vector<Token>, AST>;


static const auto grammar
= *((pe::terminal(TokenKind::A) >> pe::terminal(TokenKind::Equal) >> pe::terminal(TokenKind::B) >> pe::terminal(TokenKind::Semicolumn))->*AST::Assignment
    | (pe::terminal(TokenKind::A) >> pe::terminal(TokenKind::B) >> pe::terminal(TokenKind::Semicolumn))->*AST::Declaration)
    ;


void test_custom_token() {
    std::vector<Token> input;
    // A = B;
    input.push_back(Token{ TokenKind::A, "", 0, 0 });
    input.push_back(Token{ TokenKind::Equal, "", 0, 0 });
    input.push_back(Token{ TokenKind::B, "", 0, 0 });
    input.push_back(Token{ TokenKind::Semicolumn, "", 0, 0 });

    // A B;
    input.push_back(Token{ TokenKind::A, "", 0, 0 });
    input.push_back(Token{ TokenKind::B, "", 0, 0 });
    input.push_back(Token{ TokenKind::Semicolumn, "", 0, 0 });

    auto [parser_success, ast, parser_it, error] = pe::parse(input, grammar);

    assert(ast.size() == 2);
    assert(ast[0]->get_id() == AST::Assignment);
    assert(ast[1]->get_id() == AST::Declaration);

    for (const auto& element : ast) {
        element->print(std::cout);
    }
}
