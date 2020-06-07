#include "calculator_grammar.hpp"


using namespace parserlib;


namespace calculator
{ 


    static auto sign = one_of("-+");
    static auto digits = +range('0', '9');
    static auto num_basic_part = digits >> -('.' >> -digits) | '.' >> digits;
    static auto num_exp_part = one_of("eE") >> -sign >> digits;


    //number
    static rule<> num = -sign >> num_basic_part >> -num_exp_part  == "num";


    //value
    static auto val = '(' >> expr >> ')'
                    | num;


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
