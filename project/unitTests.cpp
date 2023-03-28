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


static void unitTest_MatchParser() {
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


Rule<> mul = (mul >> terminalSet('*', '/') >> num) == std::string("mul")
           | num;


Rule<> add = (add >> terminalSet('+', '-') >> mul) == std::string("add")
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
        else if (m.id() == "add") {
            assert(stack.size() >= 2);
            const int v2 = stack.back();
            stack.pop_back();
            const int v1 = stack.back();
            stack.pop_back();
            stack.push_back(v1 + v2);
        }
    }
    assert(stack.size() == 1);
    return stack.back();
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


void runUnitTests() {
    unitTest_AndParser();
    unitTest_ChoiceParser();
    unitTest_Loop1Parser();
    unitTest_LoopParser();
    unitTest_MatchParser();
    unitTest_NotParser();
    unitTest_OptionalParser();
    unitTest_Rule();
    unitTest_sequenceParser();
    unitTest_terminalParser();
    unitTest_terminalRangeParser();
    unitTest_terminalSetParser();
    unitTest_terminalStringParser();
    unitTest_directLeftRecursion();
}