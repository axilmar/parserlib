/*
 * EBNF grammar, taken from
 * https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form
 */


#include "parserlib.hpp"
#include "ebnf.hpp"


namespace parserlib::ebnf {


    using EBNFParseContext = ParseContext<std::string, EBNF, LineCountingSourcePosition<std::string>>;


    extern const Rule<EBNFParseContext> alternation;


    static const auto letter = terminalRange('a', 'z') 
                             | terminalRange('A', 'Z');


    static const auto digit = terminalRange('0', '9');


    static const auto symbol = terminalSet(
        '[', ']', '{', '}', '(', ')', '<', '>',
        '\'', '\"', '=', '|', '.', ',', ';', '-',
        '+', '*', '?', '\n', '\t', '\r', '\f', '\b');


    static const auto character = letter | digit | symbol | '_' | ' ';


    static const auto identifier = (letter >> *(letter | digit | '_' | '-')) == EBNF::IDENTIFIER;


    static const auto S = *(terminalSet(' ', '\n', '\t', '\r', '\f', '\b'));


    static const auto comment = "(*" >> *(character - "*)") >> "*)";


    static const auto WS = *(comment | S);


    static const auto terminal = ('\'' >> +(character - '\'') >> '\''
                               | '\"' >> +(character - '\"') >> '\"') == EBNF::TERMINAL;


    static const auto terminator = terminalSet(';', '.');


    static const auto grouped_term = ('(' >> WS >> alternation >> ')' >> WS) >= EBNF::TERM_GROUPED;


    static const auto optional_term = ('[' >> WS >> alternation >> ']' >> WS) >= EBNF::TERM_OPTIONAL;


    static const auto repeated_term = ('{' >> WS >> alternation >> '}' >> WS) >= EBNF::TERM_REPEATED;


    static const auto term = grouped_term
                           | optional_term
                           | repeated_term
                           | terminal >> WS
                           | identifier >> WS;


    static const auto optional_term_postfix = (term >> '?' >> WS) >= EBNF::TERM_OPTIONAL_POSTFIX;


    static const auto repeated_term_0_postfix = term >> '*' >> WS >= EBNF::TERM_REPEATED_0_OR_MORE_POSTFIX;


    static const auto repeated_term_1_postfix = (term >> '+' >> WS) >= EBNF::TERM_REPEATED_1_OR_MORE_POSTFIX;


    static const auto exception = (term >> '-' >> WS >> term) >= EBNF::EXCEPTION;


    static const auto factor = optional_term_postfix
                             | repeated_term_0_postfix
                             | repeated_term_1_postfix
                             | exception
                             | term;


    static const auto concatenation = (factor >> *(',' >> WS >> factor)) >= EBNF::CONCATENATION;


    static const Rule<EBNFParseContext> alternation = (concatenation >> *('|' >> WS >> concatenation)) >= EBNF::ALTERNATION;


    static const auto rule = (WS >> identifier >> WS >> '=' >> WS >> alternation >> terminator) >= EBNF::RULE;


    static const auto ebnf = *rule;


} //namespace parserlib::ebnf


