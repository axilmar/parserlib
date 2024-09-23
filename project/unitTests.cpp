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
    const auto parser = terminal('a') | 'b' | 'c';

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


static void unitTest_Loop0Parser() {
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


static void unitTest_LoopNParser() {
    {
        bool exception = false;
        try {
            const auto parser = 0 * terminal('a');
        }
        catch (const std::invalid_argument&) {
            exception = true;
        }
        assert(exception);
    }

    {
        const auto parser = 1 * terminal('a');

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
            assert(pc.sourcePosition() == std::next(input.begin()));
        }
    }

    {
        const auto parser = 2 * terminal('a');

        {
            const std::string input = "a";
            ParseContext pc(input);
            bool ok = parser(pc);
            assert(!ok);
            assert(pc.sourcePosition() == input.begin());
        }

        {
            const std::string input = "aa";
            ParseContext pc(input);
            bool ok = parser(pc);
            assert(ok);
            assert(pc.sourcePosition() == input.end());
        }
    }

    {
        const auto parser = 2 * (3 * terminal('a'));
        assert(parser.loopCount() == 6);
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
    const Rule<> rule = 'a' >> (rule | 'b')
                      | 'b' >> ('a' | rule);

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
    {
        const auto parser = terminal('a') >> 'b' >> 'c';

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

    {
        const auto parser = *(terminalRange('a', 'z') - 'q');

        {
            const std::string input = "abcdefghijklmnoprstuvwxyz";
            ParseContext pc(input);
            bool ok = parser(pc);
            assert(ok);
            assert(pc.sourcePosition() == input.end());
        }

        {
            const std::string input = "abcdefghijklmnopqrstuvwxyz";
            ParseContext pc(input);
            bool ok = parser(pc);
            assert(ok);
            assert(pc.sourcePosition() == std::next(input.begin(), 16));
        }
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
    using Match = typename ParseContext<std::string, TYPE>::MatchType;

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


bool operator == (const ParseContext<>::MatchType& treeMatch, const TreeMatchT& tm) {
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


bool operator == (const std::vector<ParseContext<>::MatchType>& treeMatches, const TreeMatchT& tm) {
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


static int eval(const ParseContext<>::MatchType& m) {
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


static void unitTest_lineCountingSourcePosition() {
    const auto ws = -terminalSet(' ', '\n');
    const auto grammar = (terminal('a') == "a") >> ws >> (terminal('b') == "b") >> ws >> (terminal('c') == "c");

    const std::string input = "ab\nc";
    ParseContext<std::string, std::string, LineCountingSourcePosition<std::string>> pc(input);

    const bool ok = grammar(pc);
    assert(ok);
    assert(pc.matches().size() == 3);
    assert(pc.matches()[0].content() == "a" && pc.matches()[0].begin().line() == 1 && pc.matches()[0].begin().column() == 1);
    assert(pc.matches()[1].content() == "b" && pc.matches()[1].begin().line() == 1 && pc.matches()[1].begin().column() == 2);
    assert(pc.matches()[2].content() == "c" && pc.matches()[2].begin().line() == 2 && pc.matches()[2].begin().column() == 1);
}


static void unitTest_errorHandling() {
    const auto parser = terminal('a') >> 'b' >> 'd' >> 'e'
                      | terminal('a') >> 'b' >> 'c' >> 'd';

    {
        const std::string input = "abcd";
        ParseContext<std::string, std::string, LineCountingSourcePosition<>> pc(input);
        const bool ok = parser(pc);
        assert(ok);
        assert(pc.sourceEnded());
        assert(pc.errors().size() == 0);
    }

    {
        const std::string input = "abcf";
        ParseContext<std::string, std::string, LineCountingSourcePosition<>> pc(input);
        const bool ok = parser(pc);
        assert(!ok);
        assert(!pc.sourceEnded());
        assert(pc.errors().size() == 1);
        assert(pc.errors()[0].position().line() == 1);
        assert(pc.errors()[0].position().column() == 4);
    }

    {
        const std::string input = "abef";
        ParseContext<std::string, std::string, LineCountingSourcePosition<>> pc(input);
        const bool ok = parser(pc);
        assert(!ok);
        assert(!pc.sourceEnded());
        assert(pc.errors().size() == 1);
        assert(pc.errors()[0].position().line() == 1);
        assert(pc.errors()[0].position().column() == 3);
    }
}


static void unitTest_errorRecovery() {
    const auto ws = *terminal(' ');
    const auto letter = terminalRange('a', 'z') | terminalRange('A', 'Z');
    const auto digit = terminalRange('0', '9');
    const auto character = letter | digit;
    const auto terminal_ = ('\'' >> *(character - '\'') >> ~terminal('\'')) == "terminal";
    const auto grammar = ws >> *(terminal_ >> ws);

    {
        const std::string input = "'abc' '123' 'abc123'";
        ParseContext<std::string, std::string, LineCountingSourcePosition<>> pc(input);
        const bool ok = grammar(pc);
        assert(ok);
        assert(pc.sourceEnded());
        assert(pc.matches().size() == 3);
        assert(pc.errors().size() == 0);
    }

    {
        const std::string input = "'@abc' '123' 'abc123'";
        ParseContext<std::string, std::string, LineCountingSourcePosition<>> pc(input);
        const bool ok = grammar(pc);
        assert(ok);
        assert(pc.sourceEnded());
        assert(pc.matches().size() == 3);
        assert(pc.errors().size() == 1);
        assert(pc.errors()[0].position().line() == 1);
        assert(pc.errors()[0].position().column() == 2);
    }

    {
        const std::string input = "'abc' '1@23' 'abc123'";
        ParseContext<std::string, std::string, LineCountingSourcePosition<>> pc(input);
        const bool ok = grammar(pc);
        assert(ok);
        assert(pc.sourceEnded());
        assert(pc.matches().size() == 3);
        assert(pc.errors().size() == 1);
        assert(pc.errors()[0].position().line() == 1);
        assert(pc.errors()[0].position().column() == 9);
    }

    {
        const std::string input = "'abc' '123' 'abc123@'";
        ParseContext<std::string, std::string, LineCountingSourcePosition<>> pc(input);
        const bool ok = grammar(pc);
        assert(ok);
        assert(pc.sourceEnded());
        assert(pc.matches().size() == 3);
        assert(pc.errors().size() == 1);
        assert(pc.errors()[0].position().line() == 1);
        assert(pc.errors()[0].position().column() == 20);
    }

    {
        const std::string input = "'a@bc' '1@23' 'abc@123'";
        ParseContext<std::string, std::string, LineCountingSourcePosition<>> pc(input);
        const bool ok = grammar(pc);
        assert(ok);
        assert(pc.sourceEnded());
        assert(pc.matches().size() == 3);
        assert(pc.errors().size() == 3);
        assert(pc.errors()[0].position().line() == 1);
        assert(pc.errors()[0].position().column() == 3);
        assert(pc.errors()[1].position().line() == 1);
        assert(pc.errors()[1].position().column() == 10);
        assert(pc.errors()[2].position().line() == 1);
        assert(pc.errors()[2].position().column() == 19);
    }
}


namespace unit_test_cfe {


    //token type
    enum class TokenType {
        Number,
        Addition,
        Subtraction,
        Multiplication,
        Division,
        LeftParen,
        RightParen
    };


    //token type output
    template <class T>
    inline std::basic_ostream<T, std::char_traits<T>>& operator << (std::basic_ostream<T, std::char_traits<T>>& stream, TokenType tokenType) {
        stream << static_cast<int>(tokenType);
        return stream;
    }


    //ast type
    enum class ASTType {
        Number,
        Addition,
        Subtraction,
        Multiplication,
        Division
    };


    //ast type output
    template <class T>
    inline std::basic_ostream<T, std::char_traits<T>>& operator << (std::basic_ostream<T, std::char_traits<T>>& stream, ASTType astType) {
        stream << static_cast<int>(astType);
        return stream;
    }


    //CFE type
    using CalculatorCFE = CFE<TokenType, ASTType>;


    //rule type
    using Rule = CalculatorCFE::RuleType;


    //ast node ptr type
    using ASTNodePtr = CalculatorCFE::ASTNodePtr;


    /**** tokenizer ****/


    //whitespace
    auto whitespace = terminalRange((char)0, ' ');


    //digit
    auto digit = terminalRange('0', '9');


    //integer
    auto integer = +digit;


    //number token
    auto number_tk = (integer >> -('.' >> integer)) == TokenType::Number;


    //operators
    auto op_add = terminal('+') == TokenType::Addition;
    auto op_sub = terminal('-') == TokenType::Subtraction;
    auto op_mul = terminal('*') == TokenType::Multiplication;
    auto op_div = terminal('/') == TokenType::Division;


    //parentheses
    auto left_paren = terminal('(') == TokenType::LeftParen;
    auto right_paren = terminal(')') == TokenType::RightParen;


    //tokenizer grammar
    auto tokenizerGrammar = *(whitespace
        | number_tk
        | op_add
        | op_sub
        | op_mul
        | op_div
        | left_paren
        | right_paren);


    /**** parser ****/


    extern const Rule& parserGrammar;


    //number rule
    auto number = terminal(TokenType::Number) >= ASTType::Number;


    //value is left parenthesis-expression-right parenthesis or number
    auto value = terminal(TokenType::LeftParen) >> parserGrammar >> terminal(TokenType::RightParen)
               | number;


    //multiplication/division
    Rule mul = (mul >> terminal(TokenType::Multiplication) >> value) >= ASTType::Multiplication
             | (mul >> terminal(TokenType::Division)       >> value) >= ASTType::Division
             |  value;


    //addition/subtraction
    Rule add = (add >> terminal(TokenType::Addition)    >> mul) >= ASTType::Addition
             | (add >> terminal(TokenType::Subtraction) >> mul) >= ASTType::Subtraction
             |  mul;


    //the main parser rule
    const Rule& parserGrammar = add;


    //evaluate result
    static double eval(const ASTNodePtr& node) {
        switch (node->id()) {
            case ASTType::Addition:
                return eval(node->children()[0]) + eval(node->children()[1]);

            case ASTType::Division:
                return eval(node->children()[0]) / eval(node->children()[1]);

            case ASTType::Multiplication:
                return eval(node->children()[0]) * eval(node->children()[1]);

            case ASTType::Number:
                return stod(node->getSource());

            case ASTType::Subtraction:
                return eval(node->children()[0]) - eval(node->children()[1]);
        }
        throw std::logic_error("Invalid ast node id");
    }


    //the unit test
    static void unitTest_CFE() {
        //CFE instance
        CalculatorCFE calculatorCFE;

        //simple integer example
        {
            std::string input = "1";
            auto [ok, ast, errors] = calculatorCFE.parse(input, tokenizerGrammar, parserGrammar);
            assert(eval(ast[0]) == 1);
        }

        //simple double example
        {
            std::string input = "1.5";
            auto [ok, ast, errors] = calculatorCFE.parse(input, tokenizerGrammar, parserGrammar);
            assert(eval(ast[0]) == 1.5);
        }

        //addition
        {
            std::string input = "1 + 2";
            auto [ok, ast, errors] = calculatorCFE.parse(input, tokenizerGrammar, parserGrammar);
            assert(eval(ast[0]) == 1 + 2);
        }

        //subtraction
        {
            std::string input = "2 - 1";
            auto [ok, ast, errors] = calculatorCFE.parse(input, tokenizerGrammar, parserGrammar);
            assert(eval(ast[0]) == 2 - 1);
        }

        //multiplication
        {
            std::string input = "2 * 3";
            auto [ok, ast, errors] = calculatorCFE.parse(input, tokenizerGrammar, parserGrammar);
            assert(eval(ast[0]) == 2 * 3);
        }

        //division
        {
            std::string input = "6 / 2";
            auto [ok, ast, errors] = calculatorCFE.parse(input, tokenizerGrammar, parserGrammar);
            assert(eval(ast[0]) == 6 / 2);
        }

        //complex example
        {
            std::string input = "1 + 2 * 3";
            auto [ok, ast, errors] = calculatorCFE.parse(input, tokenizerGrammar, parserGrammar);
            assert(eval(ast[0]) == 1 + 2 * 3);
        }

        //complex example
        {
            std::string input = "(1 + 2) * 3";
            auto [ok, ast, errors] = calculatorCFE.parse(input, tokenizerGrammar, parserGrammar);
            assert(eval(ast[0]) == (1 + 2) * 3);
        }

        //complex example
        {
            std::string input = "3 + (2 * 1)";
            auto [ok, ast, errors] = calculatorCFE.parse(input, tokenizerGrammar, parserGrammar);
            assert(eval(ast[0]) == 3 + (2 * 1));
        }

        //complex example
        {
            std::string input = "3 + (2 * (5 + 6))";
            auto [ok, ast, errors] = calculatorCFE.parse(input, tokenizerGrammar, parserGrammar);
            const auto result = eval(ast[0]);
            assert(result == 3 + (2 * (5 + 6)));
        }

        //complex example
        {
            std::string input = "(7 + 3) / 2 * (6 + 7)";
            auto [ok, ast, errors] = calculatorCFE.parse(input, tokenizerGrammar, parserGrammar);
            const auto result = eval(ast[0]);
            assert(result == (7 + 3) / 2 * (6 + 7));
        }

        {
            std::string input = "3 + ((5 + 6) * 1) / 32 * (64 + 7 / 13)";
            auto [ok, ast, errors] = calculatorCFE.parse(input, tokenizerGrammar, parserGrammar);
            const auto result1 = eval(ast[0]);
            const auto result2 = 3.0 + ((5.0 + 6.0) * 1.0) / 32.0 * (64.0 + 7.0 / 13.0);
            assert(result1 == result2);
        }
    }


} //namespace unit_test_cfe


void runUnitTests() {
    unitTest_AndParser();
    unitTest_ChoiceParser();
    unitTest_Loop0Parser();
    unitTest_Loop1Parser();
    unitTest_LoopNParser();
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
    unitTest_lineCountingSourcePosition();
    unitTest_errorHandling();
    unitTest_errorRecovery();
    unit_test_cfe::unitTest_CFE();
}
