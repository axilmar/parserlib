#include <iostream>
#include "parserlib.hpp"


//using namespace parserlib;


/*
enum class AST {
    Add,
    Sub,
    Mul,
    Div,
    Number
};


template <class Elem, class Traits>
std::basic_ostream<Elem, Traits>& operator << (std::basic_ostream<Elem, Traits>& stream, AST ast) {
    switch (ast) {
        case AST::Add:
            stream << "Add";
            break;

        case AST::Sub:
            stream << "Sub";
            break;

        case AST::Mul:
            stream << "Mul";
            break;

        case AST::Div:
            stream << "Div";
            break;

        case AST::Number:
            stream << "Number";
            break;
    }

    return stream;
}


typedef parserlib::SourceString<> SourceString;
typedef parserlib::ParseContext<AST> ParseContext;
typedef parserlib::Rule<ParseContext> Rule;


extern Rule add;


auto digit = parserlib::oneIn('0', '9');


auto num = +digit ->* AST::Number;


auto val = '(' >> add >> ')'
         | num;


Rule mul = (mul >> '*' >> val) ->* AST::Mul
         | (mul >> '/' >> val) ->* AST::Div
         | val;      


Rule add = (add >> '+' >> mul) ->* AST::Add
         | (add >> '-' >> mul) ->* AST::Sub
         | mul;


void test1() {
    SourceString src = "3*((1+2)*4)";
    ParseContext pc(src);
    bool ok = add.parse(pc);
    pc.getMatches()[0].print(std::cout);
}
*/


extern void runUnitTests();


int main() {
    runUnitTests();
    system("pause");
    return 0;
}
