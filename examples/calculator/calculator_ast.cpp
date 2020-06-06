#include "calculator_ast.hpp"


using namespace parserlib;


const match_table<> calculator_match_table
{
	{"num", ast<ast_num>()},
	{"div", ast<ast_div>()},
	{"mul", ast<ast_mul>()},
	{"sub", ast<ast_sub>()},
	{"add", ast<ast_add>()},
};
