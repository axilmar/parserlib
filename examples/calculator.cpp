#include <sstream>
#include <cassert>
#include <stdexcept>
#include <iostream>
#include "parserlib.hpp"


using namespace parserlib;


//the parser
using p = parser<>;


//the match ids
enum MATCH_ID {
    NUM,
    ADD,
    SUB,
    MUL,
    DIV
};


//forward reference to expression
extern p::rule expression;


//digit
static auto digit 
    = p::range('0', '9')
    ;


//an integer is a list of digits
static auto integer 
    = +digit
    ;


//sign is plus or minus
static auto sign 
    = p::term('+') 
    | '-'
    ;


//the integer part of a number
static auto integer_part 
    = -sign >> integer
    ;


//the decimal part of a number
static auto decimal_part 
    = '.' >> integer
    ;


//the exponent part of a number
static auto exponent_part 
    = (p::term('e') | 'E') >> -sign >> integer
    ;


//a number is composed of 3 parts:
//-the integer part, which is obligatory
//-the decimal part, which is optional
//-the exponent part, which is also optional
static auto number 
    = (integer_part >> -decimal_part >> -exponent_part) ->* NUM
    ;


//a parenthesized expression
static auto parenthesized_expression
    = '(' >> expression >> ')'
    ;


//a value is either a number or a parenthesized expression
static auto value
    = number
    | parenthesized_expression
    ;


//multiplicative expression
static auto mul
    = p::left_associative(
        value,
        '*' >> value >> p::match(MUL),
        '/' >> value >> p::match(DIV)
    );


//additive expression
static auto add
    = p::left_associative(
        mul,
        '+' >> mul >> p::match(ADD),
        '-' >> mul >> p::match(SUB)
    );


//the top level expression
static p::rule expression
    = add
    ;


//the grammar
static auto grammar 
    = expression >> p::end()
    ;


//evaluate a match
static double eval(const p::match_instance& match) {
    switch (match.get_id()) {
        case NUM: {
            std::stringstream stream;
            stream << match.get_source();
            double result;
            stream >> result;
            return result;
        }

        case ADD:
            assert(match.get_children().size() == 2);
            return eval(match.get_children()[0]) + eval(match.get_children()[1]);

        case SUB:
            assert(match.get_children().size() == 2);
            return eval(match.get_children()[0]) - eval(match.get_children()[1]);

        case MUL:
            assert(match.get_children().size() == 2);
            return eval(match.get_children()[0]) * eval(match.get_children()[1]);

        case DIV:
            assert(match.get_children().size() == 2);
            return eval(match.get_children()[0]) / eval(match.get_children()[1]);
    }

    throw std::logic_error("invalid match id");
}


void calculate(const std::string& arithmetic_expression, double expected_result) {
    p::parse_context pc{ arithmetic_expression };
    const bool ok = grammar.parse(pc);
    assert(ok);
    assert(pc.get_matches().size() == 1);
    double computed_result = eval(pc.get_matches()[0]);
    assert(computed_result == expected_result);
    std::cout << arithmetic_expression << " = " << computed_result << std::endl;
}


#define CALCULATE(EXPR) calculate(#EXPR, EXPR)


void run_example_calculator() {
    std::cout << "Parserlib example: calculator\n";
    CALCULATE(1.0);
    CALCULATE(1.0+2.0);
    CALCULATE(1.0+2.0-3.0);
    CALCULATE(1.0+2.0-3.0*4.0);
    CALCULATE(1.0+2.0-3.0*4.0/5.0);
    CALCULATE(1.0+(2.0-(3.0*4.0/5.0)));
    CALCULATE(((1.0+2.0-3.0)*4.0)/5.0);
    CALCULATE(-2.0+-1.0);
    CALCULATE(1e+10);
    CALCULATE(1e+10+2e+10);
    std::cout << "\n";
}
