#include <iostream>
#include "parserlib.hpp"
#include "parserlib/CFE_EBNF.hpp"


using namespace parserlib;


extern void runUnitTests();


void test() {
    CFE_EBNF<> ebnf;
}


int main() {
    test();
    //runUnitTests();
    system("pause");
    return 0;
}
