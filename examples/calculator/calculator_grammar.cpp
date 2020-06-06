#include "calculator_grammar.hpp"


using namespace parserlib;


//number
static auto sign = one_of("-+");
static auto digits = +range('0', '9');
static auto basic_part = digits >> -('.' >> -digits) | '.' >> digits;
static auto exp_part = one_of("eE") >> -sign >> digits;
static auto num = -sign >> basic_part >> -exp_part >= "num";


//value
static rule<> val = '(' >> calculator_expr >> ')'
                  | num;


//multiplication/division
static rule<> mul = mul >> '*' >> val >= "mul"
                  | mul >> '/' >> val >= "div"
                  | val;


//addition/subtraction
static rule<> add = add >> '+' >> mul >= "add"
                  | add >> '-' >> mul >= "sub"
                  | mul;


//expression
rule<> calculator_expr = add;
