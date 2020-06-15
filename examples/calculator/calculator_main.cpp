#include <iostream>
#include <string>
#include "calculator_ast.hpp"


using namespace parserlib;


int main()
{
    std::cout << "calculator example.\n";
    std::cout << "Expressions types can contain floating point numbers, +,-,*,% parentheses and spaces.\n";
    
    for(;;)
    {
        std::cout << "Please type expression (or press enter to exit): ";
        
        std::string input;
        std::getline(std::cin, input);
        
        if (input.empty())
        {
            break;
        }
        
        auto pc = parse_context(input);
        const auto res = parse(calculator::expr, pc);

        if (res)
        {
            std::shared_ptr<calculator::ast_expr> root = calculator::create_ast(pc);
            const double result = root->eval();            
            std::cout << "result = " << result << std::endl;
        }
        
        else
        {
            pc.position = pc.furthest_position;
            std::cout << "syntax error at: " << pc.remaining_input() << '\n';
        }
    }
    
    return 0;
}
