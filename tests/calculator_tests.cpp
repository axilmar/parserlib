#include <iostream>
#include "examples/calculator/calculator_grammar.hpp"
#include "examples/calculator/calculator_ast.hpp"


using namespace parserlib;


static void do_test(const std::string& input, const double expected_result, int &success_count, int &test_count)
{
	std::cout << "Running test: \"" << input << "\"...";

	++test_count;

	auto pc = parse_context(input);
	const auto res = parse(calculator::expr, pc);
	const auto remaining_input = pc.remaining_input();

	if (res)
	{
		std::shared_ptr<calculator::ast_expr> root = calculator::create_ast(pc);
		const double result = root->eval();
		if (result == expected_result)
		{
			std::cout << "OK\n";
			++success_count;
		}
		else
		{
			std::cout << "ERROR: result = " << result << ", expected result = " << expected_result << '\n';
		}
	}
	else
	{
		std::cout << "ERROR: parse result = " << res << ", remaining input = \"" << remaining_input << "\"\n";
	}
}


#define STRINGIFY1(S) #S
#define STRINGIFY(S) STRINGIFY1(S)


#define DO_TEST(EXPR) do_test(STRINGIFY(EXPR), EXPR, success_count, test_count)


void run_calculator_tests()
{
	std::cout << "Running calculator tests...\n";
	int success_count = 0;	
	int test_count = 0;

	DO_TEST(1.0);

	DO_TEST(1.0+2.0);
	DO_TEST(1.0-2.0);
	DO_TEST(1.0*2.0);
	DO_TEST(1.0/2.0);

	DO_TEST(1.0+2.0+3.0);
	DO_TEST(1.0-2.0-3.0);
	DO_TEST(1.0*2.0*3.0);
	DO_TEST(1.0/2.0/3.0);

	DO_TEST(1.0-2.0+3.0);
	DO_TEST(1.0+2.0-3.0);	
	DO_TEST(1.0*2.0+3.0);
	DO_TEST(1.0+2.0*3.0);
	DO_TEST(1.0/2.0+3.0);
	DO_TEST(1.0+2.0/3.0);
	DO_TEST(1.0*2.0-3.0);
	DO_TEST(1.0-2.0*3.0);
	DO_TEST(1.0/2.0-3.0);
	DO_TEST(1.0-2.0/3.0);
	DO_TEST(1.0/2.0*3.0);
	DO_TEST(1.0*2.0/3.0);

	DO_TEST((1.0));
	DO_TEST((1.0/2.0));
	DO_TEST((1.0/2.0)*3.0);
	DO_TEST(1.0/(2.0*3.0));
	DO_TEST(1.0/((2.0-3.0)*4.0));

    DO_TEST(1.0+2.0*3.0+4.0);

	std::cout << "Calculator tests finished.\n";
	std::cout << "Tests run: " << test_count << '\n';
	std::cout << "Tests succeeded: " << success_count << '\n';
	std::cout << "Tests failed: " << (test_count - success_count) << '\n';
}
