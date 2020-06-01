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
    system("pause");
    return 0;
}
