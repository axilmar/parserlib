#include "calculator_grammar.hpp"
#include <iostream>


using namespace parserlib;


//number
static auto sign = one_of("-+");
static auto digits = +range('0', '9');
static auto basic_part = digits >> -('.' >> -digits) | '.' >> digits;
static auto exp_part = one_of("eE") >> -sign >> digits;
static auto num = -sign >> basic_part >> -exp_part             >= "num";


//value
static rule<> val = '(' >> calculator_expr >> ')'
                  | num;


auto mul_event = [](parse_context<>& pc, parse_result r)
{
	if (r == parse_result::accepted)
	{
		std::cout << "mul: " << std::string_view(&*pc.start_position, pc.position - pc.start_position) << std::endl;
	}
	return r;
};


//multiplication/division
static rule<> mul = mul >> '*' >> val                          >= "mul" >= &mul_event
                  | mul >> '/' >> val                          >= "div"
                  | val;


//addition/subtraction
static rule<> add = add >> '+' >> mul                          >= "add"
                  | add >> '-' >> mul                          >= "sub"
                  | mul;


//expression
rule<> calculator_expr = add;
