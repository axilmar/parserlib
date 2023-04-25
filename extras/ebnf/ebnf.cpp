/*
 * EBNF grammar, taken from
 * https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form
 */


#include "parserlib.hpp"


namespace parserlib::ebnf {


    extern const Rule<> rhs;


    static const auto letter = terminalRange('a', 'z') 
                             | terminalRange('A', 'Z');


    static const auto digit = terminalRange('0', '9');


    static const auto non_zero_digit = terminalRange('1', '9');


    static const auto positive_integer = non_zero_digit >> *digit;


    static const auto symbol = terminalSet(
        '[', ']', '{', '}', '(', ')', '<', '>',
        '\'', '\"', '=', '|', '.', ',', ';', '-',
        '+', '*', '?', '\n', '\t', '\r', '\f', '\b');


    static const auto character = letter | digit | symbol | '_' | ' ';


    static const auto identifier = letter >> *(letter | digit | '_');


    static const auto S = *(terminalSet(' ', '\n', '\t', '\r', '\f', '\b'));


    static const auto terminal = '\'' >> +(character - '\'') >> '\''
                               | '\"' >> +(character - '\"') >> '\"';


    static const auto terminator = terminalSet(';', '.');


    static const auto term = '(' >> S >> rhs >> S >> ')'
                           | '[' >> S >> rhs >> S >> ']'
                           | '{' >> S >> rhs >> S >> '}'
                           | terminal
                           | identifier;


    static const auto factor = term >> S >> '?'
                             | term >> S >> '*'
                             | term >> S >> '+'
                             | term >> S >> '-' >> S >> term
                             | term >> S;


    static const auto repeated_factor = positive_integer >> '*' >> factor
                                      | factor;


    static const auto concatenation = S >> repeated_factor >> S >> *(',' >> S >> repeated_factor >> S);


    static const auto alternation = S >> concatenation >> S >> *('|' >> S >> concatenation >> S);


    static const Rule<> rhs = alternation;


    static const auto lhs = identifier;


    static const auto rule = lhs >> S >> '=' >> S >> rhs >> S >> terminator;


    static const auto grammar = *(S >> rule >> S);


} //namespace parserlib::ebnf


