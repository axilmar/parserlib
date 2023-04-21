#include <limits>
#include "tokenizer.hpp"
#include "parserlib.hpp"


using namespace parserlib;


/******************************************************************************
    UTILITIES
 ******************************************************************************/


//pair
template <class F, class S> struct Pair {
    F first;
    S second;
};


 //keyword table
static constexpr Pair<const char*, TOKEN_TYPE> keywordTable[] = {
    {"typedef", TOKEN_KEYWORD_TYPEDEF},
    {"double", TOKEN_KEYWORD_DOUBLE},
    {"struct", TOKEN_KEYWORD_STRUCT},
    {"write", TOKEN_KEYWORD_WRITE},
    {"enum", TOKEN_KEYWORD_ENUM},
    {"char", TOKEN_KEYWORD_CHAR},
    {"read", TOKEN_KEYWORD_READ},
    {"int", TOKEN_KEYWORD_INT},
    {"new", TOKEN_KEYWORD_NEW},
};


//create token from character
static const auto charToken(Char c, TOKEN_TYPE tokenType) {
    return terminal(c) == tokenType;
}


//keyword token
static const auto keywordToken(const Char* kw, TOKEN_TYPE tokenType) {
    return terminal(kw) == tokenType;
}


//reserved word
template <size_t Index>
static auto reservedWord() {
    if constexpr (Index < (sizeof(keywordTable) / sizeof(Pair<const char*, TOKEN_TYPE>)) - 1) {
        return terminal(keywordTable[Index].first) | reservedWord<Index + 1>();
    }
    else {
        return terminal(keywordTable[Index].first);
    }
}


//keyword
template <size_t Index>
static auto keyword_() {
    if constexpr (Index < (sizeof(keywordTable) / sizeof(Pair<const char*, TOKEN_TYPE>)) - 1) {
        return keywordToken(keywordTable[Index].first, keywordTable[Index].second) | keyword_<Index + 1>();
    }
    else {
        return keywordToken(keywordTable[Index].first, keywordTable[Index].second);
    }
}


/******************************************************************************
    GRAMMAR
 ******************************************************************************/


//digit
static const auto digit = terminalRange('0', '9');


//any character except two characters
static const auto anyCharExcept(Char c1, Char c2) {
    return !terminalSet(c1, c2) >> terminalRange((Char)1, (Char)std::numeric_limits<Char>::max());
}


//any character except a string
static const auto anyCharExcept(const Char* s) {
    return !terminal(s) >> terminalRange((Char)1, (Char)std::numeric_limits<Char>::max());
}


//line comment
static const auto lineComment = terminal("//") >> *anyCharExcept('\n', '\0') >> (terminal('\n') | eof());


//block comment
static const auto blockComment = terminal("/*") >> *anyCharExcept("*/") >> terminal("*/");


//whitespace
static const auto whitespace = terminal(' ') | terminal('\n') | lineComment | blockComment;


//letter
static const auto letter = terminalRange('a', 'z') | terminalRange('A', 'Z');


//integer literal
static const auto integerLiteral = -terminalSet('+', '-') >> +digit;


//float literal
static const auto floatLiteral = *digit >> terminal('.') >> +digit >> -(terminalSet('e', 'E') >> -terminalSet('+', '-') >> +digit);


//escaped string character
static const auto escapedStringCharacter = terminal('\\') >> terminalSet('\\', '\"', '\'', '0', 'n', 't');


//string character
static const auto stringCharacter = escapedStringCharacter
| (!terminal('\\') >> terminalRange((Char)1, (Char)std::numeric_limits<Char>::max()));


//any string character
static const auto anyStringCharacterExcept(Char c) {
    return !terminal(c) >> stringCharacter;
}


//string literal
static const auto stringLiteral = terminal('"') >> *anyStringCharacterExcept('"') >> terminal('"');


//character literal
static const auto charLiteral = terminal('\'') >> anyStringCharacterExcept('\'') >> terminal('\'');


//reserved word
static const auto reserved_word = reservedWord<0>();


//keyword
static const auto keyword = keyword_<0>();


//identifier
static const auto identifier = reserved_word >> +(letter | digit | terminal('_'))
                             | (letter | terminal('_')) >> *(letter | digit | terminal('_'));


//token
static const auto token = whitespace
                        | identifier == TOKEN_IDENTIFIER
                        | keyword
                        | floatLiteral == TOKEN_LITERAL_FLOAT
                        | integerLiteral == TOKEN_LITERAL_INTEGER
                        | stringLiteral == TOKEN_LITERAL_STRING
                        | charLiteral == TOKEN_LITERAL_CHAR
                        | charToken('~', TOKEN_TILDE)
                        | charToken('!', TOKEN_EXCLAMATION_MARK)
                        | charToken('%', TOKEN_PERCENT)
                        | charToken('&', TOKEN_AMBERSAND)
                        | charToken('*', TOKEN_STAR)
                        | charToken('(', TOKEN_OPENING_PARENTHESIS)
                        | charToken(')', TOKEN_CLOSING_PARENTHESIS)
                        | charToken('_', TOKEN_UNDERSCORE)
                        | charToken('-', TOKEN_MINUS)
                        | charToken('+', TOKEN_PLUS)
                        | charToken('=', TOKEN_EQUALS)
                        | charToken('{', TOKEN_OPENING_CURLY_BRACKET)
                        | charToken('}', TOKEN_CLOSING_CURLY_BRACKET)
                        | charToken('[', TOKEN_OPENING_SQUARE_BRACKET)
                        | charToken(']', TOKEN_CLOSING_SQUARE_BRACKET)
                        | charToken('|', TOKEN_PIPE)
                        | charToken(';', TOKEN_SEMICOLON)
                        | charToken('<', TOKEN_LESS_THAN)
                        | charToken('>', TOKEN_GREATER_THAN)
                        | charToken(',', TOKEN_COMMA)
                        | charToken('.', TOKEN_DOT)
                        | charToken('?', TOKEN_QUESTION_MARK)
                        | charToken('/', TOKEN_DIV)
                        ;


//tokenizer
static const auto tokenizer = *token;


//syntax checker
static const auto syntaxChecker = &token;


/******************************************************************************
    INTERFACE
 ******************************************************************************/


 //tokenize source
std::vector<Token> tokenize(const SourceType& input, std::vector<Error>& errors) {
    //the source view over the source type
    SourceView<SourceType> view(input);

    //declare the parse context
    ParseContext<SourceView<SourceType>, TOKEN_TYPE> pc(view);

    while (!pc.sourceEnded()) {
        //tokenize
        const bool ok = tokenizer(pc);

        //if no error, stop
        if (ok && pc.sourceEnded()) {
            break;
        }

        //find error range
        const auto startPosition = pc.sourcePosition();
        while (!pc.sourceEnded() && !syntaxChecker(pc)) {
            pc.incrementSourcePosition();
        }
        const auto endPosition = pc.sourcePosition();

        //add an error
        Error error;
        error.position.line = startPosition.line();
        error.position.column = startPosition.column();
        error.message = "Syntax error";
        error.content = std::string(startPosition, endPosition);
        errors.push_back(std::move(error));
    }

    //extract tokens
    std::vector<Token> result;
    for (const auto& match : pc.matches()) {
        result.push_back(Token{ match.id(), SourceType(match.begin(), match.end()), Position{ match.begin().line(), match.begin().column()} });
    }

    return result;
}


