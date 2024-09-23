#include <iostream>
#include "parserlib.hpp"
#include "parserlib/CFE_EBNF.hpp"


using namespace parserlib;


extern void runUnitTests();


void test(const std::string& filename) {
    const std::string input = loadASCIIFile(filename);

    CFE_EBNF<> ebnf;

    auto [success, astNodes, errors] = ebnf.parse(input);

    for (const auto& astNode : astNodes) {
        astNode->print(std::cout);
    }
}


int main(int argc, char** argv) {
    test(argv[1]);
    //runUnitTests();
    system("pause");
    return 0;
}
