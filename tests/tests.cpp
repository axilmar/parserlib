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
        const std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        const std::string input = "b";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.parse_position().iterator() == input.begin());
    }
}


static void test_parse_string() {
    const auto grammar = terminal("abc");

    {
        const std::string input = "abc";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        const std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.parse_position().iterator() == input.begin());
    }
}


static void test_parse_symbol_set() {
    const auto grammar = set("0123456789");

    {
        const std::string input = "0";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        const std::string input = "5";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        const std::string input = "9";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        const std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.parse_position().iterator() == input.begin());
    }
}


static void test_parse_symbol_range() {
    const auto grammar = range('0', '9');

    {
        const std::string input = "0";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        const std::string input = "5";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        const std::string input = "9";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        const std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.parse_position().iterator() == input.begin());
    }
}


static void test_parse_0_or_more() {
    const auto grammar = *terminal('a');

    {
        const std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        const std::string input = "aa";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        const std::string input = "aaa";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        const std::string input = "b";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.begin());
    }

    {
        const std::string input = "ab";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == std::next(input.begin()));
    }

    {
        const std::string input = "aab";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == std::next(input.begin(), 2));
    }
}


static void test_parse_1_or_more() {
    const auto grammar = +terminal('a');

    {
        const std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        const std::string input = "aa";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        const std::string input = "aaa";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        const std::string input = "b";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.parse_position().iterator() == input.begin());
    }

    {
        const std::string input = "ab";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == std::next(input.begin()));
    }

    {
        const std::string input = "aab";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == std::next(input.begin(), 2));
    }
}


static void test_parse_optional() {
    const auto grammar = -terminal('a');

    {
        const std::string input = "";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        const std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        const std::string input = "b";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.begin());
    }
}


static void test_parse_logical_and() {
    const auto grammar = &terminal('a');

    {
        const std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.begin());
    }

    {
        const std::string input = "b";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.parse_position().iterator() == input.begin());
    }
}


static void test_parse_logical_not() {
    const auto grammar = !terminal('a');

    {
        const std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.parse_position().iterator() == input.begin());
    }

    {
        const std::string input = "b";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.begin());
    }
}


static void test_parse_sequence() {
    const auto grammar = terminal('a') >> 'b' >> 'c';

    {
        const std::string input = "abc";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        const std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.parse_position().iterator() == input.begin());
    }

    {
        const std::string input = "ab";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.parse_position().iterator() == input.begin());
    }
}


static void test_parse_choice() {
    const auto grammar = terminal('a') | 'b' | 'c';

    {
        const std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        const std::string input = "b";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        const std::string input = "c";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        const std::string input = "1";
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
        const std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
        assert(pc.matches().size() == 1);
        assert(pc.matches()[0].match_id() == A);
    }

    {
        const std::string input = "ca";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
        assert(pc.matches().size() == 2);
        assert(pc.matches()[0].match_id() == C);
        assert(pc.matches()[1].match_id() == A);
    }

    {
        const std::string input = "cbdea";
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
            const std::string input = "a";
            parse_context<> pc(input);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_position().iterator() == input.end());
        }

        {
            const std::string input = "b";
            parse_context<> pc(input);
            const bool result = grammar.parse(pc);
            assert(!result);
            assert(pc.parse_position().iterator() == input.begin());
        }
    }

    {
        rule<> grammar = "abc";

        {
            const std::string input = "abc";
            parse_context<> pc(input);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_position().iterator() == input.end());
        }

        {
            const std::string input = "a";
            parse_context<> pc(input);
            const bool result = grammar.parse(pc);
            assert(!result);
            assert(pc.parse_position().iterator() == input.begin());
        }
    }

    {
        rule<> grammar = set("0123456789");

        {
            const std::string input = "0";
            parse_context<> pc(input);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_position().iterator() == input.end());
        }

        {
            const std::string input = "5";
            parse_context<> pc(input);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_position().iterator() == input.end());
        }

        {
            const std::string input = "9";
            parse_context<> pc(input);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_position().iterator() == input.end());
        }

        {
            const std::string input = "a";
            parse_context<> pc(input);
            const bool result = grammar.parse(pc);
            assert(!result);
            assert(pc.parse_position().iterator() == input.begin());
        }
    }

    {
        rule<> grammar = range('0', '9');

        {
            const std::string input = "0";
            parse_context<> pc(input);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_position().iterator() == input.end());
        }

        {
            const std::string input = "5";
            parse_context<> pc(input);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_position().iterator() == input.end());
        }

        {
            const std::string input = "9";
            parse_context<> pc(input);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_position().iterator() == input.end());
        }

        {
            const std::string input = "a";
            parse_context<> pc(input);
            const bool result = grammar.parse(pc);
            assert(!result);
            assert(pc.parse_position().iterator() == input.begin());
        }
    }

    {
        rule<> grammar = terminal('a') >> 'b' >> 'c';

        {
            const std::string input = "abc";
            parse_context<> pc(input);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_position().iterator() == input.end());
        }

        {
            const std::string input = "a";
            parse_context<> pc(input);
            const bool result = grammar.parse(pc);
            assert(!result);
            assert(pc.parse_position().iterator() == input.begin());
        }

        {
            const std::string input = "ab";
            parse_context<> pc(input);
            const bool result = grammar.parse(pc);
            assert(!result);
            assert(pc.parse_position().iterator() == input.begin());
        }
    }
}


#define TEST_CALC(V)\
{\
    const std::string input = #V;\
    parse_context<> pc(input);\
    const bool result = expr.parse(pc);\
    assert(result);\
    assert(pc.parse_position().iterator() == input.end());\
    const double v = eval_matches(pc.matches());\
    assert(v == (V));\
}


#define TEST_CALC_WITH_DEBUG_INFO(V)\
{\
    const std::string input = #V;\
    parse_context<parse_context_traits<std::string, true>> pc(input);\
    const bool result = expr.parse(pc);\
    assert(result);\
    assert(pc.parse_position().iterator() == input.end());\
    const double v = eval_matches(pc.matches());\
    assert(v == (V));\
}


template <class ParseContext = parse_context<>>
class calculator {
private:
    rule<ParseContext> expr;
    rule<ParseContext> mul;
    rule<ParseContext> add;

public:
    enum EXPR_ID {
        NUM,
        ADD,
        SUB,
        MUL,
        DIV
    };

    calculator() {
        const auto digit
            = range('0', '9');

        const auto number
            = ((+digit >> -('.' >> +digit))->*NUM)["num"];

        const auto val
            = number
            | '(' >> expr >> ')';

        mul = ((mul >> '*' >> val)->*MUL)["mul"]
            | ((mul >> '/' >> val)->*DIV)["div"]
            | val;

        add = ((add >> '+' >> mul)->*ADD)["add"]
            | ((add >> '-' >> mul)->*SUB)["sub"]
            | mul;

        expr = add;
    }

    template <class T>
    static double eval_match(const T& match) {
        switch (match.match_id()) {
            case NUM: {
                assert(match.matches().size() == 0);
                std::stringstream stream;
                stream << match.source();
                double v;
                stream >> v;
                return v;
            }

            case ADD:
                assert(match.matches().size() == 2);
                return eval_match(match.matches()[0]) + eval_match(match.matches()[1]);

            case SUB:
                assert(match.matches().size() == 2);
                return eval_match(match.matches()[0]) - eval_match(match.matches()[1]);

            case MUL:
                assert(match.matches().size() == 2);
                return eval_match(match.matches()[0]) * eval_match(match.matches()[1]);

            case DIV:
                assert(match.matches().size() == 2);
                return eval_match(match.matches()[0]) / eval_match(match.matches()[1]);
        }

        throw std::logic_error("invalid calculator state");
    }

    template <class T>
    static double eval_matches(const T& matches) {
        assert(matches.size() == 1);
        return eval_match(matches[0]);
    }

    void test_parse_left_recursive_rule() {
        TEST_CALC(1.0);
        TEST_CALC(1.0+2.0);
        TEST_CALC(1.0-2.0);
        TEST_CALC(1.0*2.0);
        TEST_CALC(1.0/2.0);

        TEST_CALC(1.0+2.0+3.0+4.0+5.0);
        TEST_CALC(1.0+2.0+3.0+4.0-5.0);
        TEST_CALC(1.0+2.0+3.0+4.0*5.0);
        TEST_CALC(1.0+2.0+3.0+4.0/5.0);
        TEST_CALC(1.0+2.0+3.0-4.0+5.0);
        TEST_CALC(1.0+2.0+3.0-4.0-5.0);
        TEST_CALC(1.0+2.0+3.0-4.0*5.0);
        TEST_CALC(1.0+2.0+3.0-4.0/5.0);
        TEST_CALC(1.0+2.0+3.0*4.0+5.0);
        TEST_CALC(1.0+2.0+3.0*4.0-5.0);
        TEST_CALC(1.0+2.0+3.0*4.0*5.0);
        TEST_CALC(1.0+2.0+3.0*4.0/5.0);
        TEST_CALC(1.0+2.0+3.0/4.0+5.0);
        TEST_CALC(1.0+2.0+3.0/4.0-5.0);
        TEST_CALC(1.0+2.0+3.0/4.0*5.0);
        TEST_CALC(1.0+2.0+3.0/4.0/5.0);
        TEST_CALC(1.0+2.0-3.0+4.0+5.0);
        TEST_CALC(1.0+2.0-3.0+4.0-5.0);
        TEST_CALC(1.0+2.0-3.0+4.0*5.0);
        TEST_CALC(1.0+2.0-3.0+4.0/5.0);
        TEST_CALC(1.0+2.0-3.0-4.0+5.0);
        TEST_CALC(1.0+2.0-3.0-4.0-5.0);
        TEST_CALC(1.0+2.0-3.0-4.0*5.0);
        TEST_CALC(1.0+2.0-3.0-4.0/5.0);
        TEST_CALC(1.0+2.0-3.0*4.0+5.0);
        TEST_CALC(1.0+2.0-3.0*4.0-5.0);
        TEST_CALC(1.0+2.0-3.0*4.0*5.0);
        TEST_CALC(1.0+2.0-3.0*4.0/5.0);
        TEST_CALC(1.0+2.0-3.0/4.0+5.0);
        TEST_CALC(1.0+2.0-3.0/4.0-5.0);
        TEST_CALC(1.0+2.0-3.0/4.0*5.0);
        TEST_CALC(1.0+2.0-3.0/4.0/5.0);
        TEST_CALC(1.0+2.0*3.0+4.0+5.0);
        TEST_CALC(1.0+2.0*3.0+4.0-5.0);
        TEST_CALC(1.0+2.0*3.0+4.0*5.0);
        TEST_CALC(1.0+2.0*3.0+4.0/5.0);
        TEST_CALC(1.0+2.0*3.0-4.0+5.0);
        TEST_CALC(1.0+2.0*3.0-4.0-5.0);
        TEST_CALC(1.0+2.0*3.0-4.0*5.0);
        TEST_CALC(1.0+2.0*3.0-4.0/5.0);
        TEST_CALC(1.0+2.0*3.0*4.0+5.0);
        TEST_CALC(1.0+2.0*3.0*4.0-5.0);
        TEST_CALC(1.0+2.0*3.0*4.0*5.0);
        TEST_CALC(1.0+2.0*3.0*4.0/5.0);
        TEST_CALC(1.0+2.0*3.0/4.0+5.0);
        TEST_CALC(1.0+2.0*3.0/4.0-5.0);
        TEST_CALC(1.0+2.0*3.0/4.0*5.0);
        TEST_CALC(1.0+2.0*3.0/4.0/5.0);
        TEST_CALC(1.0+2.0/3.0+4.0+5.0);
        TEST_CALC(1.0+2.0/3.0+4.0-5.0);
        TEST_CALC(1.0+2.0/3.0+4.0*5.0);
        TEST_CALC(1.0+2.0/3.0+4.0/5.0);
        TEST_CALC(1.0+2.0/3.0-4.0+5.0);
        TEST_CALC(1.0+2.0/3.0-4.0-5.0);
        TEST_CALC(1.0+2.0/3.0-4.0*5.0);
        TEST_CALC(1.0+2.0/3.0-4.0/5.0);
        TEST_CALC(1.0+2.0/3.0*4.0+5.0);
        TEST_CALC(1.0+2.0/3.0*4.0-5.0);
        TEST_CALC(1.0+2.0/3.0*4.0*5.0);
        TEST_CALC(1.0+2.0/3.0*4.0/5.0);
        TEST_CALC(1.0+2.0/3.0/4.0+5.0);
        TEST_CALC(1.0+2.0/3.0/4.0-5.0);
        TEST_CALC(1.0+2.0/3.0/4.0*5.0);
        TEST_CALC(1.0+2.0/3.0/4.0/5.0);
        TEST_CALC(1.0-2.0+3.0+4.0+5.0);
        TEST_CALC(1.0-2.0+3.0+4.0-5.0);
        TEST_CALC(1.0-2.0+3.0+4.0*5.0);
        TEST_CALC(1.0-2.0+3.0+4.0/5.0);
        TEST_CALC(1.0-2.0+3.0-4.0+5.0);
        TEST_CALC(1.0-2.0+3.0-4.0-5.0);
        TEST_CALC(1.0-2.0+3.0-4.0*5.0);
        TEST_CALC(1.0-2.0+3.0-4.0/5.0);
        TEST_CALC(1.0-2.0+3.0*4.0+5.0);
        TEST_CALC(1.0-2.0+3.0*4.0-5.0);
        TEST_CALC(1.0-2.0+3.0*4.0*5.0);
        TEST_CALC(1.0-2.0+3.0*4.0/5.0);
        TEST_CALC(1.0-2.0+3.0/4.0+5.0);
        TEST_CALC(1.0-2.0+3.0/4.0-5.0);
        TEST_CALC(1.0-2.0+3.0/4.0*5.0);
        TEST_CALC(1.0-2.0+3.0/4.0/5.0);
        TEST_CALC(1.0-2.0-3.0+4.0+5.0);
        TEST_CALC(1.0-2.0-3.0+4.0-5.0);
        TEST_CALC(1.0-2.0-3.0+4.0*5.0);
        TEST_CALC(1.0-2.0-3.0+4.0/5.0);
        TEST_CALC(1.0-2.0-3.0-4.0+5.0);
        TEST_CALC(1.0-2.0-3.0-4.0-5.0);
        TEST_CALC(1.0-2.0-3.0-4.0*5.0);
        TEST_CALC(1.0-2.0-3.0-4.0/5.0);
        TEST_CALC(1.0-2.0-3.0*4.0+5.0);
        TEST_CALC(1.0-2.0-3.0*4.0-5.0);
        TEST_CALC(1.0-2.0-3.0*4.0*5.0);
        TEST_CALC(1.0-2.0-3.0*4.0/5.0);
        TEST_CALC(1.0-2.0-3.0/4.0+5.0);
        TEST_CALC(1.0-2.0-3.0/4.0-5.0);
        TEST_CALC(1.0-2.0-3.0/4.0*5.0);
        TEST_CALC(1.0-2.0-3.0/4.0/5.0);
        TEST_CALC(1.0-2.0*3.0+4.0+5.0);
        TEST_CALC(1.0-2.0*3.0+4.0-5.0);
        TEST_CALC(1.0-2.0*3.0+4.0*5.0);
        TEST_CALC(1.0-2.0*3.0+4.0/5.0);
        TEST_CALC(1.0-2.0*3.0-4.0+5.0);
        TEST_CALC(1.0-2.0*3.0-4.0-5.0);
        TEST_CALC(1.0-2.0*3.0-4.0*5.0);
        TEST_CALC(1.0-2.0*3.0-4.0/5.0);
        TEST_CALC(1.0-2.0*3.0*4.0+5.0);
        TEST_CALC(1.0-2.0*3.0*4.0-5.0);
        TEST_CALC(1.0-2.0*3.0*4.0*5.0);
        TEST_CALC(1.0-2.0*3.0*4.0/5.0);
        TEST_CALC(1.0-2.0*3.0/4.0+5.0);
        TEST_CALC(1.0-2.0*3.0/4.0-5.0);
        TEST_CALC(1.0-2.0*3.0/4.0*5.0);
        TEST_CALC(1.0-2.0*3.0/4.0/5.0);
        TEST_CALC(1.0-2.0/3.0+4.0+5.0);
        TEST_CALC(1.0-2.0/3.0+4.0-5.0);
        TEST_CALC(1.0-2.0/3.0+4.0*5.0);
        TEST_CALC(1.0-2.0/3.0+4.0/5.0);
        TEST_CALC(1.0-2.0/3.0-4.0+5.0);
        TEST_CALC(1.0-2.0/3.0-4.0-5.0);
        TEST_CALC(1.0-2.0/3.0-4.0*5.0);
        TEST_CALC(1.0-2.0/3.0-4.0/5.0);
        TEST_CALC(1.0-2.0/3.0*4.0+5.0);
        TEST_CALC(1.0-2.0/3.0*4.0-5.0);
        TEST_CALC(1.0-2.0/3.0*4.0*5.0);
        TEST_CALC(1.0-2.0/3.0*4.0/5.0);
        TEST_CALC(1.0-2.0/3.0/4.0+5.0);
        TEST_CALC(1.0-2.0/3.0/4.0-5.0);
        TEST_CALC(1.0-2.0/3.0/4.0*5.0);
        TEST_CALC(1.0-2.0/3.0/4.0/5.0);
        TEST_CALC(1.0*2.0+3.0+4.0+5.0);
        TEST_CALC(1.0*2.0+3.0+4.0-5.0);
        TEST_CALC(1.0*2.0+3.0+4.0*5.0);
        TEST_CALC(1.0*2.0+3.0+4.0/5.0);
        TEST_CALC(1.0*2.0+3.0-4.0+5.0);
        TEST_CALC(1.0*2.0+3.0-4.0-5.0);
        TEST_CALC(1.0*2.0+3.0-4.0*5.0);
        TEST_CALC(1.0*2.0+3.0-4.0/5.0);
        TEST_CALC(1.0*2.0+3.0*4.0+5.0);
        TEST_CALC(1.0*2.0+3.0*4.0-5.0);
        TEST_CALC(1.0*2.0+3.0*4.0*5.0);
        TEST_CALC(1.0*2.0+3.0*4.0/5.0);
        TEST_CALC(1.0*2.0+3.0/4.0+5.0);
        TEST_CALC(1.0*2.0+3.0/4.0-5.0);
        TEST_CALC(1.0*2.0+3.0/4.0*5.0);
        TEST_CALC(1.0*2.0+3.0/4.0/5.0);
        TEST_CALC(1.0*2.0-3.0+4.0+5.0);
        TEST_CALC(1.0*2.0-3.0+4.0-5.0);
        TEST_CALC(1.0*2.0-3.0+4.0*5.0);
        TEST_CALC(1.0*2.0-3.0+4.0/5.0);
        TEST_CALC(1.0*2.0-3.0-4.0+5.0);
        TEST_CALC(1.0*2.0-3.0-4.0-5.0);
        TEST_CALC(1.0*2.0-3.0-4.0*5.0);
        TEST_CALC(1.0*2.0-3.0-4.0/5.0);
        TEST_CALC(1.0*2.0-3.0*4.0+5.0);
        TEST_CALC(1.0*2.0-3.0*4.0-5.0);
        TEST_CALC(1.0*2.0-3.0*4.0*5.0);
        TEST_CALC(1.0*2.0-3.0*4.0/5.0);
        TEST_CALC(1.0*2.0-3.0/4.0+5.0);
        TEST_CALC(1.0*2.0-3.0/4.0-5.0);
        TEST_CALC(1.0*2.0-3.0/4.0*5.0);
        TEST_CALC(1.0*2.0-3.0/4.0/5.0);
        TEST_CALC(1.0*2.0*3.0+4.0+5.0);
        TEST_CALC(1.0*2.0*3.0+4.0-5.0);
        TEST_CALC(1.0*2.0*3.0+4.0*5.0);
        TEST_CALC(1.0*2.0*3.0+4.0/5.0);
        TEST_CALC(1.0*2.0*3.0-4.0+5.0);
        TEST_CALC(1.0*2.0*3.0-4.0-5.0);
        TEST_CALC(1.0*2.0*3.0-4.0*5.0);
        TEST_CALC(1.0*2.0*3.0-4.0/5.0);
        TEST_CALC(1.0*2.0*3.0*4.0+5.0);
        TEST_CALC(1.0*2.0*3.0*4.0-5.0);
        TEST_CALC(1.0*2.0*3.0*4.0*5.0);
        TEST_CALC(1.0*2.0*3.0*4.0/5.0);
        TEST_CALC(1.0*2.0*3.0/4.0+5.0);
        TEST_CALC(1.0*2.0*3.0/4.0-5.0);
        TEST_CALC(1.0*2.0*3.0/4.0*5.0);
        TEST_CALC(1.0*2.0*3.0/4.0/5.0);
        TEST_CALC(1.0*2.0/3.0+4.0+5.0);
        TEST_CALC(1.0*2.0/3.0+4.0-5.0);
        TEST_CALC(1.0*2.0/3.0+4.0*5.0);
        TEST_CALC(1.0*2.0/3.0+4.0/5.0);
        TEST_CALC(1.0*2.0/3.0-4.0+5.0);
        TEST_CALC(1.0*2.0/3.0-4.0-5.0);
        TEST_CALC(1.0*2.0/3.0-4.0*5.0);
        TEST_CALC(1.0*2.0/3.0-4.0/5.0);
        TEST_CALC(1.0*2.0/3.0*4.0+5.0);
        TEST_CALC(1.0*2.0/3.0*4.0-5.0);
        TEST_CALC(1.0*2.0/3.0*4.0*5.0);
        TEST_CALC(1.0*2.0/3.0*4.0/5.0);
        TEST_CALC(1.0*2.0/3.0/4.0+5.0);
        TEST_CALC(1.0*2.0/3.0/4.0-5.0);
        TEST_CALC(1.0*2.0/3.0/4.0*5.0);
        TEST_CALC(1.0*2.0/3.0/4.0/5.0);
        TEST_CALC(1.0/2.0+3.0+4.0+5.0);
        TEST_CALC(1.0/2.0+3.0+4.0-5.0);
        TEST_CALC(1.0/2.0+3.0+4.0*5.0);
        TEST_CALC(1.0/2.0+3.0+4.0/5.0);
        TEST_CALC(1.0/2.0+3.0-4.0+5.0);
        TEST_CALC(1.0/2.0+3.0-4.0-5.0);
        TEST_CALC(1.0/2.0+3.0-4.0*5.0);
        TEST_CALC(1.0/2.0+3.0-4.0/5.0);
        TEST_CALC(1.0/2.0+3.0*4.0+5.0);
        TEST_CALC(1.0/2.0+3.0*4.0-5.0);
        TEST_CALC(1.0/2.0+3.0*4.0*5.0);
        TEST_CALC(1.0/2.0+3.0*4.0/5.0);
        TEST_CALC(1.0/2.0+3.0/4.0+5.0);
        TEST_CALC(1.0/2.0+3.0/4.0-5.0);
        TEST_CALC(1.0/2.0+3.0/4.0*5.0);
        TEST_CALC(1.0/2.0+3.0/4.0/5.0);
        TEST_CALC(1.0/2.0-3.0+4.0+5.0);
        TEST_CALC(1.0/2.0-3.0+4.0-5.0);
        TEST_CALC(1.0/2.0-3.0+4.0*5.0);
        TEST_CALC(1.0/2.0-3.0+4.0/5.0);
        TEST_CALC(1.0/2.0-3.0-4.0+5.0);
        TEST_CALC(1.0/2.0-3.0-4.0-5.0);
        TEST_CALC(1.0/2.0-3.0-4.0*5.0);
        TEST_CALC(1.0/2.0-3.0-4.0/5.0);
        TEST_CALC(1.0/2.0-3.0*4.0+5.0);
        TEST_CALC(1.0/2.0-3.0*4.0-5.0);
        TEST_CALC(1.0/2.0-3.0*4.0*5.0);
        TEST_CALC(1.0/2.0-3.0*4.0/5.0);
        TEST_CALC(1.0/2.0-3.0/4.0+5.0);
        TEST_CALC(1.0/2.0-3.0/4.0-5.0);
        TEST_CALC(1.0/2.0-3.0/4.0*5.0);
        TEST_CALC(1.0/2.0-3.0/4.0/5.0);
        TEST_CALC(1.0/2.0*3.0+4.0+5.0);
        TEST_CALC(1.0/2.0*3.0+4.0-5.0);
        TEST_CALC(1.0/2.0*3.0+4.0*5.0);
        TEST_CALC(1.0/2.0*3.0+4.0/5.0);
        TEST_CALC(1.0/2.0*3.0-4.0+5.0);
        TEST_CALC(1.0/2.0*3.0-4.0-5.0);
        TEST_CALC(1.0/2.0*3.0-4.0*5.0);
        TEST_CALC(1.0/2.0*3.0-4.0/5.0);
        TEST_CALC(1.0/2.0*3.0*4.0+5.0);
        TEST_CALC(1.0/2.0*3.0*4.0-5.0);
        TEST_CALC(1.0/2.0*3.0*4.0*5.0);
        TEST_CALC(1.0/2.0*3.0*4.0/5.0);
        TEST_CALC(1.0/2.0*3.0/4.0+5.0);
        TEST_CALC(1.0/2.0*3.0/4.0-5.0);
        TEST_CALC(1.0/2.0*3.0/4.0*5.0);
        TEST_CALC(1.0/2.0*3.0/4.0/5.0);
        TEST_CALC(1.0/2.0/3.0+4.0+5.0);
        TEST_CALC(1.0/2.0/3.0+4.0-5.0);
        TEST_CALC(1.0/2.0/3.0+4.0*5.0);
        TEST_CALC(1.0/2.0/3.0+4.0/5.0);
        TEST_CALC(1.0/2.0/3.0-4.0+5.0);
        TEST_CALC(1.0/2.0/3.0-4.0-5.0);
        TEST_CALC(1.0/2.0/3.0-4.0*5.0);
        TEST_CALC(1.0/2.0/3.0-4.0/5.0);
        TEST_CALC(1.0/2.0/3.0*4.0+5.0);
        TEST_CALC(1.0/2.0/3.0*4.0-5.0);
        TEST_CALC(1.0/2.0/3.0*4.0*5.0);
        TEST_CALC(1.0/2.0/3.0*4.0/5.0);
        TEST_CALC(1.0/2.0/3.0/4.0+5.0);
        TEST_CALC(1.0/2.0/3.0/4.0-5.0);
        TEST_CALC(1.0/2.0/3.0/4.0*5.0);
        TEST_CALC((1.0+2.0)+3.0+4.0+5.0);
        TEST_CALC(1.0+(2.0+3.0)+4.0-5.0);
        TEST_CALC(1.0+2.0+(3.0+4.0)*5.0);
        TEST_CALC(1.0+2.0+3.0+(4.0/5.0));
        TEST_CALC((1.0+2.0+3.0)-4.0+5.0);
        TEST_CALC(1.0+(2.0+3.0-4.0)-5.0);
        TEST_CALC(1.0+2.0+(3.0-4.0*5.0));
        TEST_CALC(1.0+2.0+(3.0-4.0)/5.0);
        TEST_CALC(1.0+(2.0+3.0*4.0+5.0));
        TEST_CALC((1.0+2.0+3.0*4.0)-5.0);
        TEST_CALC((1.0+2.0+3.0*4.0*5.0));

    }

    void test_parse_annotation() {
        TEST_CALC_WITH_DEBUG_INFO(1.0+2.0-3.0*4.0/5.0);
    }
};


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
    {
        calculator<> calc;
        DO_TEST(calc.test_parse_left_recursive_rule);
    }
    {
        calculator<parse_context<parse_context_traits<std::string, true>>> calc;
        DO_TEST(calc.test_parse_annotation);
    }
}
