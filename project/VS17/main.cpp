#include <iostream>
#include "parserlib.hpp"


using namespace parserlib;


extern rule<> expr;


auto num = make_range('0', '9');


rule<> val = '(' >> expr >> ')'
           | num;


rule<> mul = mul >> '*' >> val
           | mul >> '/' >> val
           | val;


rule<> add = add >> '+' >> mul
           | add >> '-' >> mul
           | mul;


rule<> expr = add;


int main(int argc, char* argv[])
{
    const std::string input = "1+2*3-4/5";
    auto pc = make_parse_context(input);
    const auto res = expr.parse(pc);

    std::cout << "result = " << (int)res << std::endl;
    std::cout << "remaining input = " << pc.get_remaining_input() << std::endl;

    system("pause");
    return 0;
}
