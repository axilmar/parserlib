#ifndef PARSERLIB__EXAMPLES__CALCULATOR__CALCULATOR_GRAMMAR__HPP
#define PARSERLIB__EXAMPLES__CALCULATOR__CALCULATOR_GRAMMAR__HPP


#include "parserlib.hpp"


namespace calculator
{


	//rules for the calculator
	extern parserlib::rule<> expr;
	extern parserlib::rule<> add;
	extern parserlib::rule<> sub;
	extern parserlib::rule<> mul;
	extern parserlib::rule<> div;
	extern parserlib::rule<> num;


} //namespace calculator


#endif //PARSERLIB__EXAMPLES__CALCULATOR__CALCULATOR_GRAMMAR__HPP
