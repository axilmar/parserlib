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
    const auto res = parse(calculator_expr, pc);
    const auto remaining_input = pc.remaining_input();
    
    /*
    std::cout << "result = " << (int)res << std::endl;
    std::cout << "remaining input = " << remaining_input << std::endl;
    for (const auto& match : pc.matches)
    {
        std::cout << match.tag << " => " << match << std::endl;
    }
    */

    if (res)
    {
        std::shared_ptr<ast_expr> root = std::dynamic_pointer_cast<ast_expr>(calculator_match_table.create_ast(pc.matches));
        const double result = root->eval();
        std::cout << "result = " << result << std::endl;
        std::cout << "expected = " << (TEST_EXPR) << std::endl;
    }

    system("pause");
    return 0;
}
