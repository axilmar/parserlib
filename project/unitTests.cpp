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
    const Rule<> rule = terminal('a') >> (rule | terminal('b'));

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
    const Rule<> r = 'x' >> r >> 'b'
                   | 'a';

    {
        const std::string input = "a";
        ParseContext<> pc(input);
        bool ok = r(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        const std::string input = "xab";
        ParseContext<> pc(input);
        bool ok = r(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
    }

    {
        const std::string input = "xxabb";
        ParseContext<> pc(input);
        bool ok = r(pc);
        assert(ok);
        assert(pc.sourcePosition() == input.end());
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


bool operator == (const ParseContext<>::Match& treeMatch, const TreeMatchT& tm) {
    if (treeMatch.id() != tm.id) {
        return false;
    }

    if (treeMatch.children().size() != tm.children.size()) {
        return false;
    }

    for (size_t i = 0; i < tm.children.size(); ++i) {
        if (!(treeMatch.children()[i] == tm.children[i])) {
            return false;
        }
    }

    return true;
}


bool operator == (const std::vector<ParseContext<>::Match>& treeMatches, const TreeMatchT& tm) {
    return treeMatches.size() == 1 && treeMatches[0] == tm;
}


extern Rule<> add;


static auto digit = terminalRange('0', '9');


static auto integer = +digit >= "int";


static auto num = integer
                | '(' >> add >> ')';


static Rule<> mul = (mul >> '*' >> num) >= "mul"
                  | (mul >> '/' >> num) >= "div"
                  | num;


static Rule<> add = (add >> '+' >> mul) >= "add"
                  | (add >> '-' >> mul) >= "sub"
                  | mul;


static int eval(const ParseContext<>::Match& m) {
    if (m.id() == "add") {
        return eval(m.children()[0]) + eval(m.children()[1]);
    }
    if (m.id() == "sub") {
        return eval(m.children()[0]) - eval(m.children()[1]);
    }
    if (m.id() == "mul") {
        return eval(m.children()[0]) * eval(m.children()[1]);
    }
    if (m.id() == "div") {
        return eval(m.children()[0]) / eval(m.children()[1]);
    }
    if (m.id() == "int") {
        std::stringstream stream;
        stream << m.content();
        int v;
        stream >> v;
        return v;
    }
    throw std::logic_error("Invalid match id");
}


static void unitTest_leftRecursion() {
    {
        Rule<> r = (r >> 'b') >= "b"
                 | (r >> 'c') >= "c"
                 | terminal('a') >= "a"
                 | terminal('d') >= "d";

        {
            const std::string input = "a";
            ParseContext pc(input);
            const bool ok = r(pc);
            assert(ok);
            assert(pc.sourceEnded());
            assert(pc.matches() == treeMatch("a"));
        }

        {
            const std::string input = "ab";
            ParseContext pc(input);
            const bool ok = r(pc);
            assert(ok);
            assert(pc.sourceEnded());
            assert(pc.matches() == treeMatch("b", treeMatch("a")));
        }

        {
            const std::string input = "abc";
            ParseContext pc(input);
            const bool ok = r(pc);
            assert(ok);
            assert(pc.sourceEnded());
            assert(pc.matches() == treeMatch("c", treeMatch("b", treeMatch("a"))));
        }

        {
            const std::string input = "acb";
            ParseContext pc(input);
            const bool ok = r(pc);
            assert(ok);
            assert(pc.sourceEnded());
            assert(pc.matches() == treeMatch("b", treeMatch("c", treeMatch("a"))));
        }

        {
        const std::string input = "abcb";
        ParseContext pc(input);
        const bool ok = r(pc);
        assert(ok);
        assert(pc.sourceEnded());
        assert(pc.matches() == treeMatch("b", treeMatch("c", treeMatch("b", treeMatch("a")))));
        }

        {
            const std::string input = "acbc";
            ParseContext pc(input);
            const bool ok = r(pc);
            assert(ok);
            assert(pc.sourceEnded());
            assert(pc.matches() == treeMatch("c", treeMatch("b", treeMatch("c", treeMatch("a")))));
        }

        {
            const std::string input = "aa";
            ParseContext pc(input);
            const bool ok = r(pc);
            assert(ok);
            assert(!pc.sourceEnded());
        }

        {
            const std::string input = "aba";
            ParseContext pc(input);
            const bool ok = r(pc);
            assert(ok);
            assert(!pc.sourceEnded());
            assert(pc.matches() == treeMatch("b", treeMatch("a")));
        }

        {
            const std::string input = "aca";
            ParseContext pc(input);
            const bool ok = r(pc);
            assert(ok);
            assert(!pc.sourceEnded());
            assert(pc.matches() == treeMatch("c", treeMatch("a")));
        }

        {
            const std::string input = "b";
            ParseContext pc(input);
            const bool ok = r(pc);
            assert(!ok);
            assert(!pc.sourceEnded());
        }

        {
            const std::string input = "c";
            ParseContext pc(input);
            const bool ok = r(pc);
            assert(!ok);
            assert(!pc.sourceEnded());
        }

        {
            const std::string input = "ba";
            ParseContext pc(input);
            const bool ok = r(pc);
            assert(!ok);
            assert(!pc.sourceEnded());
        }

        {
            const std::string input = "ca";
            ParseContext pc(input);
            const bool ok = r(pc);
            assert(!ok);
            assert(!pc.sourceEnded());
        }

        {
            const std::string input = "ad";
            ParseContext pc(input);
            const bool ok = r(pc);
            assert(ok);
            assert(!pc.sourceEnded());
            assert(pc.matches() == treeMatch("a"));
        }

        {
            const std::string input = "abd";
            ParseContext pc(input);
            const bool ok = r(pc);
            assert(ok);
            assert(!pc.sourceEnded());
            assert(pc.matches() == treeMatch("b", treeMatch("a")));
        }
    }

    {
        std::string input = "1";
        ParseContext pc(input);
        const bool ok = add(pc);
        const int r = eval(pc.matches()[0]);
        assert(r == 1);
    }

    {
        std::string input = "1+2";
        ParseContext pc(input);
        const bool ok = add(pc);
        const int r = eval(pc.matches()[0]);
        assert(r == 3);
    }

    {
        std::string input = "1+2*3";
        ParseContext pc(input);
        const bool ok = add(pc);
        const int r = eval(pc.matches()[0]);
        assert(r == 7);
    }

    {
        std::string input = "1*2+3";
        ParseContext pc(input);
        const bool ok = add(pc);
        const int r = eval(pc.matches()[0]);
        assert(r == 5);
    }

    {
        std::string input = "(1+2)*3";
        ParseContext pc(input);
        const bool ok = add(pc);
        const int r = eval(pc.matches()[0]);
        assert(r == 9);
    }

    {
        std::string input = "1*(2+3)";
        ParseContext pc(input);
        const bool ok = add(pc);
        const int r = eval(pc.matches()[0]);
        assert(r == 5);
    }

    {
        std::string input = "(1*(2+3))*4";
        ParseContext pc(input);
        const bool ok = add(pc);
        const int r = eval(pc.matches()[0]);
        assert(r == 20);
    }
}


void runUnitTests() {
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
    //unitTest_recursion();
    unitTest_leftRecursion();
}
