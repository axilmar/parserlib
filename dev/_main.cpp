#include <iostream>
#include <string>
#include "parserlib.hpp"
using namespace std;
using namespace parserlib;


extern rule expr, add, mul;
rule wsp = *term(' ');
rule digit = range('0', '9');
rule num = term(+digit);
rule val = num | '(' >> expr >> ')';
rule mul = mul >> '*' >> val
         | mul >> '/' >> val
         | val;
rule add = add >> '+' >> mul
         | add >> '-' >> mul
         | mul;
rule expr = add;


int main() {
    string src = "1 + 2 * 3";
    input_buffer input;
    input.insert(input.end(), src.begin(), src.end());
    bool ok = parse(input, expr, wsp);
    cout << ok;
    getchar();
    return 0;
}
