#include "calculator_grammar.hpp"


namespace calculator
{ 
    using namespace parserlib;


    static auto sign = one_of("-+");
    static auto digits = +range('0', '9');
    static auto num_basic_part = digits >> -('.' >> -digits) | '.' >> digits;
    static auto num_exp_part = one_of("eE") >> -sign >> digits;


    //number
    static auto num = -sign >> num_basic_part >> -num_exp_part  == "num";


    //whitespace
    static auto ws = *terminal(' ');


    //value
    static auto val = ws >> ('(' >> expr >> ')' | num) >> ws;


    //multiplication/division
    static rule<> mul = mul >> '*' >> val                         == "mul"
                      | mul >> '/' >> val                         == "div"
                      | val;


    //addition/subtraction
    static rule<> add = add >> '+' >> mul                         == "add"
                      | add >> '-' >> mul                         == "sub"
                      | mul;


    //expression
    rule<> expr = add;


} //namespace calculator
