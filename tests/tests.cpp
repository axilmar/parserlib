#include <cstdio>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include "parserlib.hpp"


using namespace parserlib;


#ifdef assert
#undef assert
#endif


#define assert(COND)\
    if (!(COND)) {\
        throw assertion_failed(__FILE__, __LINE__, #COND);\
    }\


#define DO_TEST(TEST)\
    try {\
        TEST();\
    }\
    catch (const std::exception& ex) {\
        std::cout << ex.what() << std::endl;\
    }\
    catch (...) {\
        std::cout << "exception happened\n";\
    }


class assertion_failed : public std::exception {
public:
    assertion_failed(const char* file, int line, const char* cond) : m_message(get_message(file, line, cond)) {
    }

    const char* what() const noexcept override {
        return m_message.c_str();
    }

private:
    std::string m_message;

    static std::string get_message(const char* file, int line, const char* cond) {
        std::stringstream stream;
        stream << "Assertion failed at file " << file << ", line " << line << ": " << cond;
        return stream.str();
    }
};


static void test_parse_symbol() {
    const auto grammar = terminal('a');

    {
        std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "b";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.parse_position().iterator() == input.begin());
    }
}


static void test_parse_string() {
    const auto grammar = terminal("abc");

    {
        std::string input = "abc";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.parse_position().iterator() == input.begin());
    }
}


static void test_parse_symbol_set() {
    const auto grammar = set("0123456789");

    {
        std::string input = "0";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "5";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "9";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.parse_position().iterator() == input.begin());
    }
}


static void test_parse_symbol_range() {
    const auto grammar = range('0', '9');

    {
        std::string input = "0";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "5";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "9";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.parse_position().iterator() == input.begin());
    }
}


static void test_parse_0_or_more() {
    const auto grammar = *terminal('a');

    {
        std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "aa";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "aaa";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "b";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.begin());
    }

    {
        std::string input = "ab";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == std::next(input.begin()));
    }

    {
        std::string input = "aab";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == std::next(input.begin(), 2));
    }
}


static void test_parse_1_or_more() {
    const auto grammar = +terminal('a');

    {
        std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "aa";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "aaa";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "b";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.parse_position().iterator() == input.begin());
    }

    {
        std::string input = "ab";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == std::next(input.begin()));
    }

    {
        std::string input = "aab";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == std::next(input.begin(), 2));
    }
}


static void test_parse_optional() {
    const auto grammar = -terminal('a');

    {
        std::string input = "";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "b";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.begin());
    }
}


static void test_parse_logical_and() {
    const auto grammar = &terminal('a');

    {
        std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.begin());
    }

    {
        std::string input = "b";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.parse_position().iterator() == input.begin());
    }
}


static void test_parse_logical_not() {
    const auto grammar = !terminal('a');

    {
        std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.parse_position().iterator() == input.begin());
    }

    {
        std::string input = "b";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.begin());
    }
}


static void test_parse_sequence() {
    const auto grammar = terminal('a') >> 'b' >> 'c';

    {
        std::string input = "abc";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.parse_position().iterator() == input.begin());
    }

    {
        std::string input = "ab";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.parse_position().iterator() == input.begin());
    }
}


static void test_parse_choice() {
    const auto grammar = terminal('a') | 'b' | 'c';

    {
        std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "b";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "c";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "1";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.parse_position().iterator() == input.begin());
    }
}


static void test_parse_match() {
    enum MATCH_ID {
        A = 1,
        B,
        C,
        D,
        E,
        DE
    };

    const auto grammar = *(terminal('a')->*A | terminal('b')->*B | terminal('c')->*C | (terminal('d')->*D >> terminal('e')->*E)->*DE);

    {
        std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
        assert(pc.matches().size() == 1);
        assert(pc.matches()[0].match_id() == A);
    }

    {
        std::string input = "ca";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
        assert(pc.matches().size() == 2);
        assert(pc.matches()[0].match_id() == C);
        assert(pc.matches()[1].match_id() == A);
    }

    {
        std::string input = "cbdea";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
        assert(pc.matches().size() == 4);
        assert(pc.matches()[0].match_id() == C);
        assert(pc.matches()[1].match_id() == B);
        assert(pc.matches()[2].match_id() == DE);
        assert(pc.matches()[2].matches().size() == 2);
        assert(pc.matches()[2].matches()[0].match_id() == D);
        assert(pc.matches()[2].matches()[1].match_id() == E);
        assert(pc.matches()[3].match_id() == A);
    }
}


static void test_parse_rule() {
    {
        rule<> grammar = 'a';

        {
            std::string input = "a";
            parse_context<> pc(input);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_position().iterator() == input.end());
        }

        {
            std::string input = "b";
            parse_context<> pc(input);
            const bool result = grammar.parse(pc);
            assert(!result);
            assert(pc.parse_position().iterator() == input.begin());
        }
    }

    {
        rule<> grammar = "abc";

        {
            std::string input = "abc";
            parse_context<> pc(input);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_position().iterator() == input.end());
        }

        {
            std::string input = "a";
            parse_context<> pc(input);
            const bool result = grammar.parse(pc);
            assert(!result);
            assert(pc.parse_position().iterator() == input.begin());
        }
    }

    {
        rule<> grammar = set("0123456789");

        {
            std::string input = "0";
            parse_context<> pc(input);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_position().iterator() == input.end());
        }

        {
            std::string input = "5";
            parse_context<> pc(input);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_position().iterator() == input.end());
        }

        {
            std::string input = "9";
            parse_context<> pc(input);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_position().iterator() == input.end());
        }

        {
            std::string input = "a";
            parse_context<> pc(input);
            const bool result = grammar.parse(pc);
            assert(!result);
            assert(pc.parse_position().iterator() == input.begin());
        }
    }
}


void run_tests() {
    DO_TEST(test_parse_symbol);
    DO_TEST(test_parse_string);
    DO_TEST(test_parse_symbol_set);
    DO_TEST(test_parse_symbol_range);
    DO_TEST(test_parse_0_or_more);
    DO_TEST(test_parse_1_or_more);
    DO_TEST(test_parse_optional);
    DO_TEST(test_parse_logical_and);
    DO_TEST(test_parse_logical_not);
    DO_TEST(test_parse_sequence);
    DO_TEST(test_parse_choice);
    DO_TEST(test_parse_match);
    DO_TEST(test_parse_rule);
}
