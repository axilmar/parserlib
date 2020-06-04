#include <iostream>
#include "parserlib.hpp"


using namespace parserlib;


extern rule<> expr;


auto num = -terminal('-') >> +range('0', '9')    >= "num";


rule<> val = '(' >> expr >> ')'
           | num;


rule<> mul = mul >> '*' >> val                   >= "mul"
           | mul >> '/' >> val                   >= "div"
           | val;


rule<> add = add >> '+' >> mul                   >= "add"
           | add >> '-' >> mul                   >= "sub"
           | mul;


rule<> expr = add;


int main(int argc, char* argv[])
{
    const std::string input = "1+2-3";
    auto pc = parse_context(input);
    const auto res = expr.parse(pc);
    const auto remaining_input = pc.remaining_input();

    std::cout << "result = " << (int)res << std::endl;
    std::cout << "remaining input = " << remaining_input << std::endl;
    for (const auto& match : pc.matches)
    {
        std::cout << match.tag << " => " << match << std::endl;
    }

    system("pause");
    return 0;
}
