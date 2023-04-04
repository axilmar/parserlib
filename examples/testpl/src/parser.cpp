#include "parser.hpp"
#include "parserlib.hpp"


using namespace parserlib;


//parse context type
using ParserParseContext = ParseContext<std::vector<Token>, AST_TYPE>;


//char type
static const auto charType = terminal(TOKEN_KEYWORD_CHAR) == AST_TYPE_CHAR;


//int type
static const auto intType = terminal(TOKEN_KEYWORD_INT) == AST_TYPE_DOUBLE;


//double type
static const auto doubleType = terminal(TOKEN_KEYWORD_DOUBLE) == AST_TYPE_DOUBLE;


//named type
static const auto namedType = terminal(TOKEN_IDENTIFIER) == AST_TYPE_NAME;


//primitive type
static const auto primitiveType = charType
                                | intType
                                | doubleType;


//type
static const auto type = primitiveType
                       | namedType;


//variable name
static const auto variableName = terminal(TOKEN_IDENTIFIER) == AST_DECL_VARIABLE_NAME;


//variable declaration
static const auto variableDeclaration = 
    (type >> variableName/* >> -(terminal(TOKEN_EQUALS) >> expression)*/ >> terminal(TOKEN_SEMICOLON)) >= AST_DECL_VARIABLE;


//declaration
static const auto declaration = variableDeclaration;


//translation unit (root rule of the grammar)
static const auto translationUnit = *declaration;


//convert array of matches into an AST, pushed into a tree
static void toAST(const ParserParseContext::Match& match, std::vector<AST>& asts) {
    AST ast;
    ast.type = match.id();
    ast.tokens = std::vector<Token>(match.begin(), match.end());
    for (const auto& child : match.children()) {
        toAST(child, ast.children);
    }
    asts.push_back(std::move(ast));
}


//parse input into an ast
std::vector<AST> parse(const std::vector<Token>& tokens, std::vector<Error>& errors) {
    //prepare the parse context
    ParserParseContext pc(tokens);

    //parse
    const bool ok = translationUnit(pc);

    //convert matches to AST 
    std::vector<AST> result;
    for (const auto& match : pc.matches()) {
        toAST(match, result);
    }

    return result;
}
