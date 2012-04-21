#include <iostream>
#include <string>
#include "parserlib.hpp"
using namespace std;
using namespace parserlib;


extern rule expr, add, mul;
rule ws_ = *term(' ');
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
    ws_.set_name("ws");
    digit.set_name("digit");
    num.set_name("num");
    val.set_name("val");
    mul.set_name("mul");
    add.set_name("add");
    expr.set_name("expr");

    string src = "(1)";
    input_buffer input;
    input.insert(input.end(), src.begin(), src.end());
    bool ok = parse(input, expr, ws_);
    cout << ok;
    getchar();
    return 0;
}
