#include <iostream>
#include "parserlib/ZeroOrMore.hpp"
#include "parserlib/OneOrMore.hpp"
#include "parserlib/Optional.hpp"
#include "parserlib/LogicalAnd.hpp"
#include "parserlib/LogicalNot.hpp"
#include "parserlib/Sequence.hpp"
#include "parserlib/Choice.hpp"
#include "parserlib/Terminal.hpp"
#include "parserlib/TerminalSequence.hpp"
#include "parserlib/Range.hpp"
#include "parserlib/Set.hpp"
#include "parserlib/Rule.hpp"


using namespace std;
using namespace parserlib;


class Test : public Expression
{
};


extern Rule<> expr;


Rule<> num{ "num", +range('0', '9') };


Rule<> val{ "val", '(' >> expr >> ')'
                   | num };


Rule<> mul{ "mul", mul >> '*' >> val
                 | mul >> '/' >> val
                 | val };


Rule<> add{ "add", add >> '+' >> mul
                 | add >> '-' >> mul
                 | mul };


Rule<> expr = { "expr", add };


void test(std::string input)
{
    ParseContext<> parseContext(input);
    const bool ok = expr.parse(parseContext);
    const auto result = std::string(parseContext.getCurrentPosition(), parseContext.getEndPosition());

    cout << input << " => ";
    if (result.empty())
    {
        cout << "SUCCESS\n";
        for (auto it = parseContext.getOutput().begin(); it != parseContext.getOutput().end(); ++it)
        {
            const auto &m = *it;
            std::cout << "    " << m.getRule().getName() << " " << std::string(m.getStart(), m.getEnd()) << std::endl;
        }
    }
    else
    {
        cout << result << endl;
    }
    cout << endl;
}


void tests()
{
    test("1");
    test("1+2");
    test("1+2+3");
    test("1+2*3");
    test("1*2+3");
    test("(1+2)+3");
    test("1+(2+3)");
    test("((1)+2)+3");
    test("1+(2+(3))");
    test("((1+2)+2)+3");
    test("1+(2+(3+4))");
    test("((1*2)/2)+3");
    test("1+(2*(3-4))");
}


int main()
{
    tests();
    system("pause");
    return 0;
}
