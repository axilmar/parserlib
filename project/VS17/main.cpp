#include <iostream>
#include "examples/calculator/calculator_grammar.hpp"
#include "examples/calculator/calculator_ast.hpp"


using namespace parserlib;


#define STRINGIFY1(S) #S
#define STRINGIFY(S) STRINGIFY1(S)


#define TEST_EXPR 1+2*3


int main(int argc, char* argv[])
{
    const std::string input = STRINGIFY(TEST_EXPR);
    auto pc = parse_context(input);
    const auto res = parse(calculator::expr, pc);
    const auto remaining_input = pc.remaining_input();
    
    if (res)
    {
        std::shared_ptr<calculator::ast_expr> root = calculator::create_ast(pc);
        const double result = root->eval();
        std::cout << "result = " << result << std::endl;
        std::cout << "expected = " << (TEST_EXPR) << std::endl;
    }

    system("pause");
    return 0;
}
