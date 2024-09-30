#include <cassert>
#include <iostream>
#include <sstream>
#include "parserlib.hpp"


using namespace std;
using namespace parserlib;


static void unitTest_AndParser() {
    const auto parser = &term('a');

    {
        SourceString input = "a";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.begin());
    }

    {
        SourceString input = "b";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        assert(!ok);
        assert(pc.getCurrentPosition() == input.begin());
    }
}


static void unitTest_ChoiceParser() {
    const auto parser = term('a') | 'b' | 'c';

    {
        SourceString input = "a";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.end());
    }

    {
        SourceString input = "b";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.end());
    }

    {
        SourceString input = "c";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.end());
    }

    {
        SourceString input = "d";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        assert(!ok);
        assert(pc.getCurrentPosition() == input.begin());
    }
}


static void unitTest_ZeroOrMoreParser() {
    const auto parser = *term('a');

    {
        SourceString input = "a";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.end());
    }

    {
        SourceString input = "aa";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.end());
    }

    {
        SourceString input = "aaa";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.end());
    }

    {
        SourceString input = "b";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.begin());
    }

    {
        SourceString input = "bb";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.begin());
    }

    {
        SourceString input = "bbb";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.begin());
    }

    {
        SourceString input = "";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.begin());
    }
}


static void unitTest_OneOrMoreParser() {
    const auto parser = +term('a');

    {
        SourceString input = "a";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.end());
    }

    {
        SourceString input = "aa";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.end());
    }

    {
        SourceString input = "aaa";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.end());
    }

    {
        SourceString input = "b";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        assert(!ok);
        assert(pc.getCurrentPosition() == input.begin());
    }

    {
        SourceString input = "bb";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        assert(!ok);
        assert(pc.getCurrentPosition() == input.begin());
    }

    {
        SourceString input = "bbb";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        assert(!ok);
        assert(pc.getCurrentPosition() == input.begin());
    }

    {
        SourceString input = "";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        assert(!ok);
        assert(pc.getCurrentPosition() == input.begin());
    }
}


static void unitTest_NotParser() {
    const auto parser = !term('a');

    {
        SourceString input = "b";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.begin());
    }

    {
        SourceString input = "a";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        assert(!ok);
        assert(pc.getCurrentPosition() == input.begin());
    }
}


static void unitTest_OptionalParser() {
    const auto parser = -term('a');

    {
        SourceString input = "a";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.end());
    }

    {
        SourceString input = "b";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.begin());
    }
}


static void unitTest_Rule() {
    Rule<> rule = 'a' >> (rule | 'b')
                | 'b' >> ('a'  | rule);

    {
        SourceString input = "ab";
        ParseContext<> pc(input);
        bool ok = rule.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.end());
    }

    {
        SourceString input = "aab";
        ParseContext<> pc(input);
        bool ok = rule.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.end());
    }

    {
        SourceString input = "aaab";
        ParseContext<> pc(input);
        bool ok = rule.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.end());
    }

    {
        SourceString input = "b";
        ParseContext<> pc(input);
        bool ok = rule.parse(pc);
        assert(!ok);
        assert(pc.getCurrentPosition() == input.begin());
    }

    //deleted functions
    //Rule<>('a') >> 'a';
    //'a' >> Rule<>('a');
    //Rule<>('a') | 'a';
    //'a' | Rule<>('a');
    //*Rule<>('a');
    //+Rule<>('a');
    //-Rule<>('a');
    //&Rule<>('a');
    //!Rule<>('a');
    //Rule<>('a') == std::string("a");
    //Rule<>('a') >= std::string("a");
}


static void unitTest_sequenceParser() {
    {
        const auto parser = term('a') >> 'b' >> 'c';

        {
            SourceString input = "abc";
            ParseContext<> pc(input);
            bool ok = parser.parse(pc);
            assert(ok);
            assert(pc.getCurrentPosition() == input.end());
        }

        {
            SourceString input = "dabc";
            ParseContext<> pc(input);
            bool ok = parser.parse(pc);
            assert(!ok);
            assert(pc.getCurrentPosition() == input.begin());
        }

        {
            SourceString input = "adbc";
            ParseContext<> pc(input);
            bool ok = parser.parse(pc);
            assert(!ok);
            assert(pc.getCurrentPosition() == input.begin());
        }
    }

    {
        const auto parser = *(oneIn('a', 'z') - 'q');

        {
            SourceString input = "abcdefghijklmnoprstuvwxyz";
            ParseContext pc(input);
            bool ok = parser.parse(pc);
            assert(ok);
            assert(pc.getCurrentPosition() == input.end());
        }

        {
            SourceString input = "abcdefghijklmnopqrstuvwxyz";
            ParseContext pc(input);
            bool ok = parser.parse(pc);
            assert(ok);
            assert(pc.getCurrentPosition().getIterator() == input.begin().getIterator() + 16);
        }
    }
}


static void unitTest_terminalParser() {
    const auto parser = term('a');

    {
        SourceString input = "a";
        ParseContext<> pc(input);
        bool ok = parser.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.end());
    }

    {
        SourceString input = "b";
        ParseContext<> pc(input);
        bool ok = parser.parse(pc);
        assert(!ok);
        assert(pc.getCurrentPosition() == input.begin());
    }
}


static void unitTest_terminalRangeParser() {
    const auto parser = oneIn('0', '9');

    {
        SourceString input = "0";
        ParseContext<> pc(input);
        bool ok = parser.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.end());
    }

    {
        SourceString input = "b";
        ParseContext<> pc(input);
        bool ok = parser.parse(pc);
        assert(!ok);
        assert(pc.getCurrentPosition() == input.begin());
    }
}


static void unitTest_terminalSetParser() {
    const auto parser = oneOf("0123456789");

    {
        SourceString input = "0";
        ParseContext<> pc(input);
        bool ok = parser.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.end());
    }

    {
        SourceString input = "5";
        ParseContext<> pc(input);
        bool ok = parser.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.end());
    }

    {
        SourceString input = "9";
        ParseContext<> pc(input);
        bool ok = parser.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.end());
    }

    {
        SourceString input = "b";
        ParseContext<> pc(input);
        bool ok = parser.parse(pc);
        assert(!ok);
        assert(pc.getCurrentPosition() == input.begin());
    }
}


static void unitTest_terminalStringParser() {
    const auto parser = term("int");

    {
        SourceString input = "int";
        ParseContext<> pc(input);
        bool ok = parser.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.end());
    }

    {
        SourceString input = "b";
        ParseContext<> pc(input);
        bool ok = parser.parse(pc);
        assert(!ok);
        assert(pc.getCurrentPosition() == input.begin());
    }
}


static void unitTest_Match() {
    const auto parser = term('a') ->* 'm';

    {
        SourceString input = "a";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        const auto matches = pc.getMatches();
        assert(ok);
        assert(pc.getCurrentPosition() == input.end());
        assert(matches.size() == 1);
        assert(matches[0].getId() == 'm');
    }

    {
        SourceString input = "b";
        ParseContext pc(input);
        bool ok = parser.parse(pc);
        const auto matches = pc.getMatches();
        assert(!ok);
        assert(pc.getCurrentPosition() == input.begin());
        assert(matches.size() == 0);
    }
}


static void unitTest_TreeMatch() {
    enum TYPE {
        ZERO,
        ONE,
        TWO,
        THREE,
        FOUR,
        FIVE,
        SIX,
        SEVEN,
        EIGHT,
        NINE,
        A,
        B,
        C,
        D,
        E,
        F,
        HEX_DIGIT,
        HEX_BYTE,
        IP4_ADDRESS
    };

    const auto zero = term('0') ->* ZERO;
    const auto one = term('1') ->* ONE;
    const auto two = term('2') ->* TWO;
    const auto three = term('3') ->* THREE;
    const auto four = term('4') ->* FOUR;
    const auto five = term('5') ->* FIVE;
    const auto six = term('6') ->* SIX;
    const auto seven = term('7') ->* SEVEN;
    const auto eight = term('8') ->* EIGHT;
    const auto nine = term('9') ->* NINE;

    const auto a = term('A') ->* A;
    const auto b = term('B') ->* B;
    const auto c = term('C') ->* C;
    const auto d = term('D') ->* D;
    const auto e = term('E') ->* E;
    const auto f = term('F') ->* F;

    const auto hexDigit = (zero | one | two | three | four | five | six | seven | eight | nine | a | b | c | d | f) ->* HEX_DIGIT;

    const auto hexByte = (hexDigit >> hexDigit) ->* HEX_BYTE;

    const auto ip4Address = (hexByte >> term('.') >> hexByte >> term('.') >> hexByte >> term('.') >> hexByte) ->* IP4_ADDRESS;

    SourceString input = "FF.12.DC.A0";

    ParseContext<> pc(input);

    const bool ok = ip4Address.parse(pc);

    assert(ok);
    assert(pc.getMatches().size() == 1);

    const auto& match = pc.getMatches()[0];

    std::stringstream stream;
    stream << match.getChildren()[0].getChildren()[0].getSource();
    stream << match.getChildren()[0].getChildren()[1].getSource();
    stream << '.';
    stream << match.getChildren()[1].getChildren()[0].getSource();
    stream << match.getChildren()[1].getChildren()[1].getSource();
    stream << '.';
    stream << match.getChildren()[2].getChildren()[0].getSource();
    stream << match.getChildren()[2].getChildren()[1].getSource();
    stream << '.';
    stream << match.getChildren()[3].getChildren()[0].getSource();
    stream << match.getChildren()[3].getChildren()[1].getSource();
    const std::string output = stream.str();
    assert(input == output);
}


static void unitTest_recursion() {
    Rule<> r = 'x' >> r >> 'b'
             | 'a';

    {
        SourceString input = "a";
        ParseContext<> pc(input);
        bool ok = r.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.end());
    }

    {
        SourceString input = "xab";
        ParseContext<> pc(input);
        bool ok = r.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.end());
    }

    {
        SourceString input = "xxabb";
        ParseContext<> pc(input);
        bool ok = r.parse(pc);
        assert(ok);
        assert(pc.getCurrentPosition() == input.end());
    }
}


struct TreeMatchT {
    std::string id;
    std::vector<TreeMatchT> children;
};


TreeMatchT treeMatch(const std::string& s) {
    return { s, {} };
}


TreeMatchT treeMatch(const std::string& s, TreeMatchT&& child) {
    return { s, { std::move(child) } };
}


enum class AST {
    Int,
    Add,
    Sub,
    Mul,
    Div
};


using CalculatorParseContext = ParseContext<AST>;


extern Rule<CalculatorParseContext> add;


static auto digit = oneIn('0', '9');


static auto integer = +digit ->* AST::Int;


static auto num = integer
                | '(' >> add >> ')';


static Rule<CalculatorParseContext> mul = (mul >> '*' >> num) ->* AST::Mul
                                        | (mul >> '/' >> num) ->* AST::Div
                                        | num;


static Rule<CalculatorParseContext> add = (add >> '+' >> mul) ->* AST::Add
                                        | (add >> '-' >> mul) ->* AST::Sub
                                        | mul;


static int eval(const Match<AST>& m) {
    if (m.getId() == AST::Add) {
        return eval(m.getChildren()[0]) + eval(m.getChildren()[1]);
    }
    if (m.getId() == AST::Sub) {
        return eval(m.getChildren()[0]) - eval(m.getChildren()[1]);
    }
    if (m.getId() == AST::Mul) {
        return eval(m.getChildren()[0]) * eval(m.getChildren()[1]);
    }
    if (m.getId() == AST::Div) {
        return eval(m.getChildren()[0]) / eval(m.getChildren()[1]);
    }
    if (m.getId() == AST::Int) {
        std::stringstream stream;
        stream << m.getSource();
        int v;
        stream >> v;
        return v;
    }
    throw std::logic_error("Invalid match id");
}


static void unitTest_leftRecursion() {
    {
        SourceString input = "1";
        CalculatorParseContext pc(input);
        const bool ok = add.parse(pc);
        const int r = eval(pc.getMatches()[0]);
        assert(r == 1);
    }

    {
        SourceString input = "1+2";
        CalculatorParseContext pc(input);
        const bool ok = add.parse(pc);
        const int r = eval(pc.getMatches()[0]);
        assert(r == 3);
    }

    {
        SourceString input = "1+2*3";
        CalculatorParseContext pc(input);
        const bool ok = add.parse(pc);
        const int r = eval(pc.getMatches()[0]);
        assert(r == 7);
    }

    {
        SourceString input = "1*2+3";
        CalculatorParseContext pc(input);
        const bool ok = add.parse(pc);
        const int r = eval(pc.getMatches()[0]);
        assert(r == 5);
    }

    {
        SourceString input = "(1+2)*3";
        CalculatorParseContext pc(input);
        const bool ok = add.parse(pc);
        const int r = eval(pc.getMatches()[0]);
        assert(r == 9);
    }

    {
        SourceString input = "1*(2+3)";
        CalculatorParseContext pc(input);
        const bool ok = add.parse(pc);
        const int r = eval(pc.getMatches()[0]);
        assert(r == 5);
    }

    {
        SourceString input = "(1*(2+3))*4";
        CalculatorParseContext pc(input);
        const bool ok = add.parse(pc);
        const int r = eval(pc.getMatches()[0]);
        assert(r == 20);
    }
}


static void unitTest_errorResumePoint() {
    auto ws = term(' ');
    auto letter = oneIn('a', 'z') | oneIn('A', 'Z');
    auto digit = oneIn('0', '9');
    auto identifier = letter >> *(letter | digit | '_');
    auto statement = identifier >> ~term(';');
    auto grammar = *(ws | statement);

    {
        SourceString input = "a; b; @; c; d; !; e;";
        ParseContext<> pc(input);
        bool ok = grammar.parse(pc);
        auto errors = pc.getErrors();
        assert(ok == true);
        assert(errors.size() == 2);
        assert(*errors[0].getStartPosition() == '@');
        assert(*errors[1].getStartPosition() == '!');
    }
}


void runUnitTests() {
    unitTest_AndParser();
    unitTest_ChoiceParser();
    unitTest_ZeroOrMoreParser();
    unitTest_OneOrMoreParser();
    unitTest_NotParser();
    unitTest_OptionalParser();
    unitTest_Rule();
    unitTest_sequenceParser();
    unitTest_terminalParser();
    unitTest_terminalRangeParser();
    unitTest_terminalSetParser();
    unitTest_terminalStringParser();
    unitTest_Match();
    unitTest_TreeMatch();
    unitTest_recursion();
    unitTest_leftRecursion();
    unitTest_errorResumePoint();
}
