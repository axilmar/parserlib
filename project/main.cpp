#include <iostream>
#include "parserlib.hpp"


using namespace parserlib;


extern void runUnitTests();


static void test() {
    enum TokenType {
        TokenA,
        TokenB,
        TokenC
    };


    enum ASTType {
        A,
        B,
        C
    };

    using CFEType = CFE<TokenType, ASTType>;

    CFEType cfe;

    auto tokenGrammar = *((terminal('a') == TokenA) | (terminal('b') == TokenB) | (terminal('c') == TokenC));

    auto c = terminal(C) >= C;
    auto a = (terminal(A) >> c) >= A;
    auto b = terminal(B) >= B;

    auto grammar = *(a | b);

    std::string source = "acb";

    auto [ok, ast, errors] = cfe.parse(source, tokenGrammar, grammar);

    printf("");
}


int main() {
    test();
    //runUnitTests();
    system("pause");
    return 0;
}
