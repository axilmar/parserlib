#include "calculator_grammar.hpp"


using namespace parserlib;


namespace calculator
{ 


	static auto sign = one_of("-+");
	static auto digits = +range('0', '9');
	static auto num_basic_part = digits >> -('.' >> -digits) | '.' >> digits;
	static auto num_exp_part = one_of("eE") >> -sign >> digits;


	//number
	rule<> num = -sign >> num_basic_part >> -num_exp_part;


	//value
	static auto val = '(' >> expr >> ')'
					| num;


	//division
	rule<> div = div >> '/' >> val
		       | val;


	//multiplication
	rule<> mul = mul >> '*' >> div
			   | div;


	//subtraction
	rule<> sub = sub >> '-' >> mul
		       | mul;


	//addition
	rule<> add = add >> '+' >> sub
			   | sub;


	//expression
	rule<> expr = add;


} //namespace calculator
