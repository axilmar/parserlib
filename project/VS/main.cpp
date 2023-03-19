#include <cassert>
#include <iostream>
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
    const auto parser = terminal('a') >>= std::string("m");

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
}


static void runUnitTests() {
    unitTest_AndParser();
    unitTest_ChoiceParser();
    unitTest_Loop1Parser();
    unitTest_LoopParser();
    unitTest_MatchParser();
    unitTest_NotParser();
    unitTest_OptionalParser();
    unitTest_Rule();
}


int main() {
    runUnitTests();
    system("pause");
    return 0;
}
