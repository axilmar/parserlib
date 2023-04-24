#ifndef PARSERLIB_EBNF_HPP
#define PARSERLIB_EBNF_HPP


/*
 * EBNF parser, taken from 
 * https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form
 */


#include "../../parserlib.hpp"


namespace parserlib::ebnf {


    extern const Rule<> rhs;


    const auto letter = terminalRange('a', 'z') | terminalRange('A', 'Z');


    const auto digit = terminalRange('0', '9');


    const auto non_zero_digit = terminalRange('1', '9');


    const auto positive_integer = non_zero_digit >> *digit;


    const auto symbol = terminalSet(
        '[' , ']' , '{', '}' , '(' , ')' , '<' , '>', 
        '\'', '\"', '=', '|' , '.' , ',' , ';' , '-', 
        '+' , '*' , '?', '\n', '\t', '\r', '\f', '\b');


    const auto character = letter | digit | symbol | '_' | ' ';


    const auto identifier = letter >> *(letter | digit | '_');


    const auto S = *(terminalSet(' ', '\n', '\t', '\r', '\f', '\b'));


    const auto terminal = '\'' >> +(character - '\'') >> '\''
                        | '\"' >> +(character - '\"') >> '\"';


    const auto terminator = terminalSet(';', '.');


    const auto term = '(' >> S >> rhs >> S >> ')'
                    | '[' >> S >> rhs >> S >> ']'
                    | '{' >> S >> rhs >> S >> '}'
                    | terminal 
                    | identifier;


    const auto factor = term >> S >> '?'
                      | term >> S >> '*'
                      | term >> S >> '+'
                      | term >> S >> '-' >> S >> term
                      | term >> S;


    const auto repeated_factor = positive_integer >> '*' >> factor 
                               | factor;


    const auto concatenation = S >> repeated_factor >> S >> *(',' >> S >> factor >> S);


    const auto alternation = S >> concatenation >> S >> *('|' >> S >> concatenation >> S);


    const Rule<> rhs = alternation;


    const auto lhs = identifier;


    const auto rule = lhs >> S >> '=' >> S >> rhs >> S >> terminator;


    const auto grammar = *(S >> rule >> S);


} //namespace parserlib::ebnf


#endif //PARSERLIB_EBNF_HPP
