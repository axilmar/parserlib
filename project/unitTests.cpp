#include <cassert>
#include <iostream>
#include <sstream>
#include "parserlib.hpp"


using namespace std;
using namespace parserlib;


static void unitTest_AndParser() {
    const auto parser = &terminal('a');

    {
        const std::string input = "a";
        ParseContext pc(input);
        bool ok = parser(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.begin());
    }

    {
        const std::string input = "b";
        ParseContext pc(input);
        bool ok = parser(pc);
        assert(!ok);
        assert(pc.sourcePosition() == input.begin());
    }
}


static void unitTest_ChoiceParser() {
    const auto parser = terminal('a') | terminal('b') | terminal('c');

    {
        const std::string input = "a";
        ParseContext pc(input);
        bool ok = parser(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        const std::string input = "b";
        ParseContext pc(input);
        bool ok = parser(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        const std::string input = "c";
        ParseContext pc(input);
        bool ok = parser(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        const std::string input = "d";
        ParseContext pc(input);
        bool ok = parser(pc);
        assert(!ok);
        assert(pc.sourcePosition() == input.begin());
    }
}


static void unitTest_Loop1Parser() {
    const auto parser = +terminal('a');

    {
        const std::string input = "a";
        ParseContext pc(input);
        bool ok = parser(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        const std::string input = "aa";
        ParseContext pc(input);
        bool ok = parser(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        const std::string input = "aaa";
        ParseContext pc(input);
        bool ok = parser(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        const std::string input = "b";
        ParseContext pc(input);
        bool ok = parser(pc);
        assert(!ok);
        assert(pc.sourcePosition() == input.begin());
    }

    {
        const std::string input = "bb";
        ParseContext pc(input);
        bool ok = parser(pc);
        assert(!ok);
        assert(pc.sourcePosition() == input.begin());
    }

    {
        const std::string input = "bbb";
        ParseContext pc(input);
        bool ok = parser(pc);
        assert(!ok);
        assert(pc.sourcePosition() == input.begin());
    }

    {
        const std::string input = "";
        ParseContext pc(input);
        bool ok = parser(pc);
        assert(!ok);
        assert(pc.sourcePosition() == input.begin());
    }
}


static void unitTest_LoopParser() {
    const auto parser = *terminal('a');

    {
        const std::string input = "a";
        ParseContext pc(input);
        bool ok = parser(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        const std::string input = "aa";
        ParseContext pc(input);
        bool ok = parser(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        const std::string input = "aaa";
        ParseContext pc(input);
        bool ok = parser(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        const std::string input = "b";
        ParseContext pc(input);
        bool ok = parser(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.begin());
    }

    {
        const std::string input = "bb";
        ParseContext pc(input);
        bool ok = parser(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.begin());
    }

    {
        const std::string input = "bbb";
        ParseContext pc(input);
        bool ok = parser(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.begin());
    }

    {
        const std::string input = "";
        ParseContext pc(input);
        bool ok = parser(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.begin());
    }
}


static void unitTest_NotParser() {
    const auto parser = !terminal('a');

    {
        const std::string input = "b";
        ParseContext pc(input);
        bool ok = parser(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.begin());
    }

    {
        const std::string input = "a";
        ParseContext pc(input);
        bool ok = parser(pc);
        assert(!ok);
        assert(pc.sourcePosition() == input.begin());
    }
}


static void unitTest_OptionalParser() {
    const auto parser = -terminal('a');

    {
        const std::string input = "a";
        ParseContext pc(input);
        bool ok = parser(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        const std::string input = "b";
        ParseContext pc(input);
        bool ok = parser(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.begin());
    }
}


static void unitTest_Rule() {
    Rule<> rule = terminal('a') >> (rule | terminal('b'));

    {
        const std::string input = "ab";
        ParseContext<> pc(input);
        bool ok = rule(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        const std::string input = "aab";
        ParseContext<> pc(input);
        bool ok = rule(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        const std::string input = "aaab";
        ParseContext<> pc(input);
        bool ok = rule(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        const std::string input = "b";
        ParseContext<> pc(input);
        bool ok = rule(pc);
        assert(!ok);
        assert(pc.sourcePosition() == input.begin());
    }
}


static void unitTest_sequenceParser() {
    const auto parser = terminal('a') >> terminal('b') >> terminal('c');

    {
        const std::string input = "abc";
        ParseContext<> pc(input);
        bool ok = parser(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        const std::string input = "dabc";
        ParseContext<> pc(input);
        bool ok = parser(pc);
        assert(!ok);
        assert(pc.sourcePosition() == input.begin());
    }

    {
        const std::string input = "adbc";
        ParseContext<> pc(input);
        bool ok = parser(pc);
        assert(!ok);
        assert(pc.sourcePosition() == input.begin());
    }
}


static void unitTest_terminalParser() {
    const auto parser = terminal('a');

    {
        const std::string input = "a";
        ParseContext<> pc(input);
        bool ok = parser(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        const std::string input = "b";
        ParseContext<> pc(input);
        bool ok = parser(pc);
        assert(!ok);
        assert(pc.sourcePosition() == input.begin());
    }
}


static void unitTest_terminalRangeParser() {
    const auto parser = terminalRange('0', '9');

    {
        const std::string input = "0";
        ParseContext<> pc(input);
        bool ok = parser(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        const std::string input = "b";
        ParseContext<> pc(input);
        bool ok = parser(pc);
        assert(!ok);
        assert(pc.sourcePosition() == input.begin());
    }
}


static void unitTest_terminalSetParser() {
    const auto parser = terminalSet('0', '1', '2', '3', '4', '5', '6', '7', '8', '9');

    {
        const std::string input = "0";
        ParseContext<> pc(input);
        bool ok = parser(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        const std::string input = "5";
        ParseContext<> pc(input);
        bool ok = parser(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        const std::string input = "9";
        ParseContext<> pc(input);
        bool ok = parser(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        const std::string input = "b";
        ParseContext<> pc(input);
        bool ok = parser(pc);
        assert(!ok);
        assert(pc.sourcePosition() == input.begin());
    }
}


static void unitTest_terminalStringParser() {
    const auto parser = terminal("int");

    {
        const std::string input = "int";
        ParseContext<> pc(input);
        bool ok = parser(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        const std::string input = "b";
        ParseContext<> pc(input);
        bool ok = parser(pc);
        assert(!ok);
        assert(pc.sourcePosition() == input.begin());
    }
}


extern Rule<> add;


const auto val = (+terminalRange('0', '9')) == std::string("num");


const auto num = val
               | terminal('(') >> add >> terminal(')');


Rule<> mul = (mul >> terminal('*') >> num) == std::string("mul")
           | (mul >> terminal('/') >> num) == std::string("div")
           | num;


Rule<> add = (add >> terminal('+') >> mul) == std::string("add")
           | (add >> terminal('-') >> mul) == std::string("sub")
           | mul;


static int to_int(const std::string& str) {
    std::stringstream stream;
    stream << str;
    int r;
    stream >> r;
    return r;
}


static int compute(const std::vector<ParseContext<>::Match>& matches) {
    std::vector<int> stack;

    for (const auto& m : matches) {
        if (m.id() == "num") {
            stack.push_back(to_int(std::string(m.begin(), m.end())));
        }
        else if (m.id() == "mul") {
            assert(stack.size() >= 2);
            const int v2 = stack.back();
            stack.pop_back();
            const int v1 = stack.back();
            stack.pop_back();
            stack.push_back(v1 * v2);
        }
        else if (m.id() == "div") {
            assert(stack.size() >= 2);
            const int v2 = stack.back();
            stack.pop_back();
            const int v1 = stack.back();
            stack.pop_back();
            stack.push_back(v1 / v2);
        }
        else if (m.id() == "add") {
            assert(stack.size() >= 2);
            const int v2 = stack.back();
            stack.pop_back();
            const int v1 = stack.back();
            stack.pop_back();
            stack.push_back(v1 + v2);
        }
        else if (m.id() == "sub") {
            assert(stack.size() >= 2);
            const int v2 = stack.back();
            stack.pop_back();
            const int v1 = stack.back();
            stack.pop_back();
            stack.push_back(v1 - v2);
        }
    }
    assert(stack.size() == 1);
    return stack.back();
}


static void unitTest_Match() {
    const auto parser = terminal('a') == std::string("m");

    {
        const std::string input = "a";
        ParseContext pc(input);
        bool ok = parser(pc);
        const auto matches = pc.matches();
        assert(ok);
        assert(pc.sourcePosition() == input.end());
        assert(matches.size() == 1);
        assert(matches[0].id() == "m");
    }

    {
        const std::string input = "b";
        ParseContext pc(input);
        bool ok = parser(pc);
        const auto matches = pc.matches();
        assert(!ok);
        assert(pc.sourcePosition() == input.begin());
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

    const auto zero = terminal('0') == ZERO;
    const auto one = terminal('1') == ONE;
    const auto two = terminal('2') == TWO;
    const auto three = terminal('3') == THREE;
    const auto four = terminal('4') == FOUR;
    const auto five = terminal('5') == FIVE;
    const auto six = terminal('6') == SIX;
    const auto seven = terminal('7') == SEVEN;
    const auto eight = terminal('8') == EIGHT;
    const auto nine = terminal('9') == NINE;

    const auto a = terminal('A') == A;
    const auto b = terminal('B') == B;
    const auto c = terminal('C') == C;
    const auto d = terminal('D') == D;
    const auto e = terminal('E') == E;
    const auto f = terminal('F') == F;

    const auto hexDigit = (zero | one | two | three | four | five | six | seven | eight | nine | a | b | c | d | f) >= HEX_DIGIT;

    const auto hexByte = (hexDigit >> hexDigit) >= HEX_BYTE;

    const auto ip4Address = (hexByte >> terminal('.') >> hexByte >> terminal('.') >> hexByte >> terminal('.') >> hexByte) >= IP4_ADDRESS;

    const std::string input = "FF.12.DC.A0";

    ParseContext<std::string, TYPE> pc(input);
    using Match = typename ParseContext<std::string, TYPE>::Match;

    const bool ok = ip4Address(pc);

    assert(ok);
    assert(pc.matches().size() == 1);

    const Match& match = pc.matches()[0];

    std::stringstream stream;
    stream << match.children()[0].children()[0].content();
    stream << match.children()[0].children()[1].content();
    stream << '.';
    stream << match.children()[1].children()[0].content();
    stream << match.children()[1].children()[1].content();
    stream << '.';
    stream << match.children()[2].children()[0].content();
    stream << match.children()[2].children()[1].content();
    stream << '.';
    stream << match.children()[3].children()[0].content();
    stream << match.children()[3].children()[1].content();
    const std::string output = stream.str();
    assert(input == output);
}


static void unitTest_recursion() {
    {
        Rule<> a = terminal('x') >> a >> terminal('b')
               |   terminal('a');
        const std::string input = "xab";
        ParseContext<> pc(input);
        bool ok = a(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        Rule<> a = terminal('x') >> a >> terminal('b')
               |   terminal('a');
        const std::string input = "xcb";
        ParseContext<> pc(input);
        bool ok = a(pc);
        assert(!ok);
        assert(pc.sourcePosition() != input.end());
    }

    {
        Rule<> r = 'x' >>  r  >> 'b'
                 |  r  >> 'c'
                 | 'a';

        {
            const std::string input = "xacb";
            ParseContext pc(input);
            const bool ok = r(pc);
            assert(ok);
            assert(pc.sourceEnded());
        }

        {
            const std::string input = "xab";
            ParseContext pc(input);
            const bool ok = r(pc);
            assert(ok);
            assert(pc.sourceEnded());
        }

        {
            const std::string input = "xabc";
            ParseContext pc(input);
            const bool ok = r(pc);
            assert(ok);
            assert(pc.sourceEnded());
        }

        {
            const std::string input = "ac";
            ParseContext pc(input);
            const bool ok = r(pc);
            assert(ok);
            assert(pc.sourceEnded());
        }
    }
}


static void unitTest_directLeftRecursion() {
    {
        Rule<> a = a >> terminal('b')
                 | terminal('a');
        const std::string input = "ab";
        ParseContext<> pc(input);
        bool ok = a(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        Rule<> a = a >> terminal('b')
                 | terminal('a');
        const std::string input = "abbb";
        ParseContext<> pc(input);
        bool ok = a(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        Rule<> a = a >> terminal('b')
                 | terminal('a');
        const std::string input = "aabbb";
        ParseContext<> pc(input);
        bool ok = a(pc);
        assert(ok);
        assert(pc.sourcePosition() != input.end());
    }

    {
        Rule<> a = a >> terminal('b')
                 | a >> terminal('c')
                 | terminal('a');
        const std::string input = "abcbcbc";
        ParseContext<> pc(input);
        bool ok = a(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        const std::string input = "1";
        ParseContext<> pc(input);
        bool ok = add(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
        assert(compute(pc.matches()) == 1);
    }

    {
        const std::string input = "1+2";
        ParseContext<> pc(input);
        bool ok = add(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
        assert(compute(pc.matches()) == 3);
    }

    {
        const std::string input = "1*2+3";
        ParseContext<> pc(input);
        bool ok = add(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
        assert(compute(pc.matches()) == 5);
    }

    {
        const std::string input = "1+2*3";
        ParseContext<> pc(input);
        bool ok = add(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
        assert(compute(pc.matches()) == 7);
    }

    {
        const std::string input = "(1*2)+3";
        ParseContext<> pc(input);
        bool ok = add(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
        assert(compute(pc.matches()) == 5);
    }

    {
        const std::string input = "1*(2+3)";
        ParseContext<> pc(input);
        bool ok = add(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
        assert(compute(pc.matches()) == 5);
    }

    {
        const std::string input = "(1*(2+3))*4+5";
        ParseContext<> pc(input);
        bool ok = add(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
        assert(compute(pc.matches()) == 25);
    }

    {
        const std::string input = "(1*2+3*4)*(5+6)";
        ParseContext<> pc(input);
        bool ok = add(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
        assert(compute(pc.matches()) == 154);
    }
}


static void unitTest_indirectLeftRecursion() {
    {
        Rule<> a = -(terminal('x')) >> a >> terminal('b')
                 | terminal('a');
        const std::string input = "ab";
        ParseContext<> pc(input);
        bool ok = a(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        Rule<> a = -(terminal('x')) >> a >> terminal('b')
                 | terminal('a');
        const std::string input = "xab";
        ParseContext<> pc(input);
        bool ok = a(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }
}


static void testR() {
}


void runUnitTests() {
    testR();
    //unitTest_AndParser();
    //unitTest_ChoiceParser();
    //unitTest_Loop1Parser();
    //unitTest_LoopParser();
    //unitTest_NotParser();
    //unitTest_OptionalParser();
    //unitTest_Rule();
    //unitTest_sequenceParser();
    //unitTest_terminalParser();
    //unitTest_terminalRangeParser();
    //unitTest_terminalSetParser();
    //unitTest_terminalStringParser();
    //unitTest_Match();
    //unitTest_TreeMatch();
    unitTest_recursion();
    //unitTest_directLeftRecursion();
    //unitTest_indirectLeftRecursion();
}
