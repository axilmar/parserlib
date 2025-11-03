#include <cassert>
#include <sstream>
#include <iostream>
#include "parserlib.hpp"


using namespace parserlib;


static void test_symbol_parsing() {
    const auto grammar = terminal('a');

    {
        std::string src = "a";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "b";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.parse_position().iterator() == src.begin());
    }
}


static void test_string_parsing() {
    const auto grammar = terminal("abc");

    {
        std::string src = "abc";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "bca";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.parse_position().iterator() == src.begin());
    }
}


static void test_set_parsing() {
    const auto grammar = set("abc");

    {
        std::string src = "a";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "b";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "c";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "1";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.parse_position().iterator() == src.begin());
    }
}


static void test_range_parsing() {
    const auto grammar = range('0', '9');

    {
        std::string src = "0";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "5";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "9";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "a";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.parse_position().iterator() == src.begin());
    }
}


static void test_any_parsing() {
    const auto grammar = any();

    {
        std::string src = "a";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "b";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.parse_position().iterator() == src.begin());
    }

}


static void test_bool_parsing() {
    {
        const auto grammar = make_parse_node(true);
        std::string src = "a";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_position().iterator() == src.begin());
    }

    {
        const auto grammar = make_parse_node(false);
        std::string src = "a";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.parse_position().iterator() == src.begin());
    }
}


static void test_end_parsing() {
    const auto grammar = end();

    {
        std::string src = "";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "a";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.parse_position().iterator() == src.begin());
    }
}


static bool parse_letter_a(parse_context<>& pc) {
    if (pc.parse_valid() && pc.compare_current_symbol('a') == 0) {
        pc.increment_parse_position();
        return true;
    }
    return false;
}


static void test_function_parsing() {
    {
        const auto grammar = make_parse_node(parse_letter_a);

        {
            std::string src = "a";
            parse_context<> pc(src);
            const bool ok = grammar.parse(pc);
            assert(ok);
            assert(pc.parse_ended());
        }

        {
            std::string src = "b";
            parse_context<> pc(src);
            const bool ok = grammar.parse(pc);
            assert(!ok);
            assert(pc.parse_position().iterator() == src.begin());
        }
    }

    {
        const auto grammar = make_parse_node([](parse_context<>& pc) {
            return parse_letter_a(pc);
        });

        {
            std::string src = "a";
            parse_context<> pc(src);
            const bool ok = grammar.parse(pc);
            assert(ok);
            assert(pc.parse_ended());
        }

        {
            std::string src = "b";
            parse_context<> pc(src);
            const bool ok = grammar.parse(pc);
            assert(!ok);
            assert(pc.parse_position().iterator() == src.begin());
        }
    }
}


static void test_newline_parsing() {
    using parse_context = parserlib::parse_context<std::string, int, int, text_position>;
    const auto grammar = newline('\n');

    {
        std::string src = "\n";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
        assert(pc.parse_position().text_position().line() == 2);
        assert(pc.parse_position().text_position().column() == 1);
    }

    {
        std::string src = "a";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.parse_position().iterator() == src.begin());
        assert(pc.parse_position().text_position().line() == 1);
        assert(pc.parse_position().text_position().column() == 1);
    }
}


static void test_loop_0_parsing() {
    const auto grammar = *terminal('a');

    {
        std::string src = "a";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "aa";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "aaa";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "b";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_position().iterator() == src.begin());
    }

    {
        std::string src = "ab";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_position().iterator() == std::next(src.begin(), 1));
    }

    {
        std::string src = "aab";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_position().iterator() == std::next(src.begin(), 2));
    }
}


static void test_loop_1_parsing() {
    const auto grammar = +terminal('a');

    {
        std::string src = "a";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "aa";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "aaa";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "b";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.parse_position().iterator() == src.begin());
    }

    {
        std::string src = "ab";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_position().iterator() == std::next(src.begin(), 1));
    }

    {
        std::string src = "aab";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_position().iterator() == std::next(src.begin(), 2));
    }
}


static void test_optional_parsing() {
    const auto grammar = -terminal('a');

    {
        std::string src = "a";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "b";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_position().iterator() == src.begin());
    }

    {
        std::string src = "";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_position().iterator() == src.begin());
    }
}


static void test_logical_and_parsing() {
    const auto grammar = &terminal('a');

    {
        std::string src = "a";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_position().iterator() == src.begin());
    }

    {
        std::string src = "b";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.parse_position().iterator() == src.begin());
    }
}


static void test_logical_not_parsing() {
    const auto grammar = !terminal('a');

    {
        std::string src = "a";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.parse_position().iterator() == src.begin());
    }

    {
        std::string src = "b";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_position().iterator() == src.begin());
    }
}


static void test_sequence_parsing() {
    const auto grammar = terminal('a') >> 'b' >> 'c';

    {
        std::string src = "abc";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "bca";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.parse_position().iterator() == src.begin());
    }
}


static void test_choice_parsing() {
    const auto grammar = terminal('a') | 'b' | 'c';

    {
        std::string src = "a";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "b";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "c";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "1";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.parse_position().iterator() == src.begin());
    }
}


static void test_match_parsing() {
    enum { A, B, C };

    const auto a = terminal('a')->*A;
    const auto b = terminal('b')->*B;
    const auto c = terminal('c')->*C;
    const auto grammar = *(a | b | c);

    {
        std::string src = "abc";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
        assert(pc.matches().size() == 3);
        assert(pc.matches()[0].id() == A);
        assert(pc.matches()[0].start_position().iterator() == src.begin());
        assert(pc.matches()[0].end_iterator() == std::next(src.begin()));
        assert(pc.matches()[1].id() == B);
        assert(pc.matches()[1].start_position().iterator() == std::next(src.begin()));
        assert(pc.matches()[1].end_iterator() == std::next(src.begin(), 2));
        assert(pc.matches()[2].id() == C);
        assert(pc.matches()[2].start_position().iterator() == std::next(src.begin(), 2));
        assert(pc.matches()[2].end_iterator() == std::next(src.begin(), 3));
    }
}


static void test_rule_parsing() {
    rule<> grammar = 'a';

    {
        std::string src = "a";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "b";
        parse_context<> pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.parse_position().iterator() == src.begin());
    }
}


#define TEST_CALC(V)\
{\
    std::string input = #V;\
    parse_context pc(input);\
    const bool result = expr.parse(pc);\
    assert(result);\
    assert(pc.parse_ended());\
    const double v = eval_matches(pc.matches());\
    assert(v == (V));\
}


class calculator {
private:
    enum EXPR_ID {
        NUM,
        ADD,
        SUB,
        MUL,
        DIV
    };

    using parse_context = parserlib::parse_context<std::string, EXPR_ID>;

    rule<parse_context> expr;
    rule<parse_context> mul;
    rule<parse_context> add;

public:
    calculator() {
        const auto digit
            = range('0', '9');

        const auto number
            = (+digit >> -('.' >> +digit))->*NUM;

        const auto val
            = number
            | '(' >> expr >> ')';

        mul = (mul >> '*' >> val)->*MUL
            | (mul >> '/' >> val)->*DIV
            | val;

        add = (add >> '+' >> mul)->*ADD
            | (add >> '-' >> mul)->*SUB
            | mul;

        expr = add;

        expr.set_name("expr");
        add.set_name("add");
        mul.set_name("mul");
    }

private:
    template <class T>
    static double eval_match(const T& match) {
        switch (match.id()) {
            case NUM: {
                assert(match.children().size() == 0);
                std::stringstream stream;
                stream << match.source();
                double v;
                stream >> v;
                return v;
            }

            case ADD:
                assert(match.children().size() == 2);
                return eval_match(match.children()[0]) + eval_match(match.children()[1]);

            case SUB:
                assert(match.children().size() == 2);
                return eval_match(match.children()[0]) - eval_match(match.children()[1]);

            case MUL:
                assert(match.children().size() == 2);
                return eval_match(match.children()[0]) * eval_match(match.children()[1]);

            case DIV:
                assert(match.children().size() == 2);
                return eval_match(match.children()[0]) / eval_match(match.children()[1]);
        }

        throw std::logic_error("invalid calculator state");
    }

    template <class T>
    static double eval_matches(const T& matches) {
        assert(matches.size() == 1);
        return eval_match(matches[0]);
    }

public:
    void test_rule_left_recursion_parsing() {
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
};


static void test_case_insensitive_parsing() {
    const auto grammar = terminal("abc");

    using parse_context = parserlib::parse_context<std::string, int, int, text_position, case_insensitive_symbol_comparator>;

    {
        std::string src = "ABC";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "abC";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "Abc";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "ABc";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
    }

    {
        std::string src = "A";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.parse_position().iterator() == src.begin());
    }

    {
        std::string src = "a";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.parse_position().iterator() == src.begin());
    }
}


static void test_non_character_parsing() {
    enum TOKEN_ID { TOKEN_A, TOKEN_B, TOKEN_C };

    struct token {
        TOKEN_ID id;
        operator TOKEN_ID () const { return id; }
    };

    enum ID { A, B, C };

    const auto a = terminal(TOKEN_A)->*A;
    const auto b = terminal(TOKEN_B)->*B;
    const auto c = terminal(TOKEN_C)->*C;
    const auto grammar = *(a | b | c);

    {
        std::vector<token> src;
        src.push_back(token{ TOKEN_A });
        src.push_back(token{ TOKEN_B });
        src.push_back(token{ TOKEN_C });

        parse_context<std::vector<token>, ID> pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.parse_ended());
        assert(pc.matches().size() == 3);
        assert(pc.matches()[0].id() == A);
        assert(pc.matches()[0].start_position().iterator() == src.begin());
        assert(pc.matches()[0].end_iterator() == std::next(src.begin()));
        assert(pc.matches()[1].id() == B);
        assert(pc.matches()[1].start_position().iterator() == std::next(src.begin()));
        assert(pc.matches()[1].end_iterator() == std::next(src.begin(), 2));
        assert(pc.matches()[2].id() == C);
        assert(pc.matches()[2].start_position().iterator() == std::next(src.begin(), 2));
        assert(pc.matches()[2].end_iterator() == std::next(src.begin(), 3));
    }
}


static void test_debug_annotations() {

    const auto nl = newline('\n');
    const auto a = terminal('a')["A"];
    const auto b = terminal('b')["B"];
    const auto c = terminal('c')["C"];
    const auto d = terminal('d')["D"];
    const auto e = terminal('e')["E"];
    const auto de = (d | e)["DE"];
    const auto grammar = *((nl | a | b | c | de)["term"]);

    {
        std::string str = "a\nbde\nc";

        parse_context<
            default_source_type,
            default_match_id_type,
            default_error_id_type,
            default_text_position_type,
            default_symbol_comparator_type,
            debug_parse_context_extension<>
        > pc(str);

        std::stringstream stream;
        pc.set_output_stream(&stream);

        const bool result = grammar.parse(pc);
        assert(result);

        const std::string result_str = stream.str();
        std::cout << result_str;
    }
}


static void test_rule_optimizations() {
    {
        rule<> grammar = 'a';
        std::string src = "a";
        parse_context<> pc(src);
        const bool result = grammar.parse(pc);
        assert(result);
    }

    {
        rule<> grammar 
            = grammar >> 'b'
            | 'a'
            | 'x';

        {
            std::string src = "a";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
        }

        {
            std::string src = "ab";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
        }

        {
            std::string src = "x";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
        }

        {
            std::string src = "xb";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
        }
    }

    {
        rule<> grammar
            = grammar >> 'b'
            | grammar >> 'c'
            | 'a'
            | 'x';

        {
            std::string src = "a";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
        }

        {
            std::string src = "ab";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
        }

        {
            std::string src = "ac";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
        }

        {
            std::string src = "x";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
        }

        {
            std::string src = "xb";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
        }

        {
            std::string src = "xc";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
        }
    }

    {
        rule<> grammar
            = grammar >> 'b'
            | grammar >> 'c'
            | grammar >> 'd'
            | 'a'
            | 'x';

        {
            std::string src = "a";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
        }

        {
            std::string src = "ab";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
        }

        {
            std::string src = "ac";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
        }

        {
            std::string src = "ad";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
        }

        {
            std::string src = "x";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
        }

        {
            std::string src = "xb";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
        }

        {
            std::string src = "xc";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
        }

        {
            std::string src = "xd";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
        }
    }

    {
        enum { A, B, X };

        rule<> grammar
            = (grammar >> 'b')->*B
            | terminal('a')->*A
            | terminal('x')->*X;

        {
            std::string src = "a";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
            assert(pc.matches().size() == 1);
            assert(pc.matches()[0].id() == A);
            assert(pc.matches()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].end_iterator() == src.end());
        }

        {
            std::string src = "ab";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
            assert(pc.matches().size() == 1);
            assert(pc.matches()[0].id() == B);
            assert(pc.matches()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].end_iterator() == src.end());
            assert(pc.matches()[0].children()[0].id() == A);
            assert(pc.matches()[0].children()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].children()[0].end_iterator() == std::next(src.begin()));
        }

        {
            std::string src = "x";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
            assert(pc.matches().size() == 1);
            assert(pc.matches()[0].id() == X);
            assert(pc.matches()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].end_iterator() == src.end());
        }

        {
            std::string src = "xb";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
            assert(pc.matches().size() == 1);
            assert(pc.matches()[0].id() == B);
            assert(pc.matches()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].end_iterator() == src.end());
            assert(pc.matches()[0].children()[0].id() == X);
            assert(pc.matches()[0].children()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].children()[0].end_iterator() == std::next(src.begin()));
        }
    }

    {
        enum { A, B, C, X };

        rule<> grammar
            = (grammar >> 'b')->*B
            | (grammar >> 'c')->*C
            | terminal('a')->*A
            | terminal('x')->*X;

        {
            std::string src = "a";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
            assert(pc.matches().size() == 1);
            assert(pc.matches()[0].id() == A);
            assert(pc.matches()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].end_iterator() == src.end());
        }

        {
            std::string src = "ab";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
            assert(pc.matches().size() == 1);
            assert(pc.matches()[0].id() == B);
            assert(pc.matches()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].end_iterator() == src.end());
            assert(pc.matches()[0].children()[0].id() == A);
            assert(pc.matches()[0].children()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].children()[0].end_iterator() == std::next(src.begin()));
        }

        {
            std::string src = "ac";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
            assert(pc.matches().size() == 1);
            assert(pc.matches()[0].id() == C);
            assert(pc.matches()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].end_iterator() == src.end());
            assert(pc.matches()[0].children()[0].id() == A);
            assert(pc.matches()[0].children()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].children()[0].end_iterator() == std::next(src.begin()));
        }

        {
            std::string src = "x";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
            assert(pc.matches().size() == 1);
            assert(pc.matches()[0].id() == X);
            assert(pc.matches()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].end_iterator() == src.end());
        }

        {
            std::string src = "xb";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
            assert(pc.matches().size() == 1);
            assert(pc.matches()[0].id() == B);
            assert(pc.matches()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].end_iterator() == src.end());
            assert(pc.matches()[0].children()[0].id() == X);
            assert(pc.matches()[0].children()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].children()[0].end_iterator() == std::next(src.begin()));
        }

        {
            std::string src = "xc";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
            assert(pc.matches()[0].id() == C);
            assert(pc.matches()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].end_iterator() == src.end());
            assert(pc.matches()[0].children()[0].id() == X);
            assert(pc.matches()[0].children()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].children()[0].end_iterator() == std::next(src.begin()));
        }
    }

    {
        enum { A, B, C, D, X };

        rule<> grammar
            = (grammar >> 'b')->*B
            | (grammar >> 'c')->*C
            | (grammar >> 'd')->*D
            | terminal('a')->*A
            | terminal('x')->*X;

        {
            std::string src = "a";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
            assert(pc.matches().size() == 1);
            assert(pc.matches()[0].id() == A);
            assert(pc.matches()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].end_iterator() == src.end());
        }

        {
            std::string src = "ab";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
            assert(pc.matches().size() == 1);
            assert(pc.matches()[0].id() == B);
            assert(pc.matches()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].end_iterator() == src.end());
            assert(pc.matches()[0].children()[0].id() == A);
            assert(pc.matches()[0].children()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].children()[0].end_iterator() == std::next(src.begin()));
        }

        {
            std::string src = "ac";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
            assert(pc.matches().size() == 1);
            assert(pc.matches()[0].id() == C);
            assert(pc.matches()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].end_iterator() == src.end());
            assert(pc.matches()[0].children()[0].id() == A);
            assert(pc.matches()[0].children()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].children()[0].end_iterator() == std::next(src.begin()));
        }

        {
            std::string src = "ad";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
            assert(pc.matches().size() == 1);
            assert(pc.matches()[0].id() == D);
            assert(pc.matches()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].end_iterator() == src.end());
            assert(pc.matches()[0].children()[0].id() == A);
            assert(pc.matches()[0].children()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].children()[0].end_iterator() == std::next(src.begin()));
        }

        {
            std::string src = "x";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
            assert(pc.matches().size() == 1);
            assert(pc.matches()[0].id() == X);
            assert(pc.matches()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].end_iterator() == src.end());
        }

        {
            std::string src = "xb";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
            assert(pc.matches().size() == 1);
            assert(pc.matches()[0].id() == B);
            assert(pc.matches()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].end_iterator() == src.end());
            assert(pc.matches()[0].children()[0].id() == X);
            assert(pc.matches()[0].children()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].children()[0].end_iterator() == std::next(src.begin()));
        }

        {
            std::string src = "xc";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
            assert(pc.matches().size() == 1);
            assert(pc.matches()[0].id() == C);
            assert(pc.matches()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].end_iterator() == src.end());
            assert(pc.matches()[0].children()[0].id() == X);
            assert(pc.matches()[0].children()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].children()[0].end_iterator() == std::next(src.begin()));
        }

        {
            std::string src = "xd";
            parse_context<> pc(src);
            const bool result = grammar.parse(pc);
            assert(result);
            assert(pc.parse_ended());
            assert(pc.matches().size() == 1);
            assert(pc.matches()[0].id() == D);
            assert(pc.matches()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].end_iterator() == src.end());
            assert(pc.matches()[0].children()[0].id() == X);
            assert(pc.matches()[0].children()[0].start_position().iterator() == src.begin());
            assert(pc.matches()[0].children()[0].end_iterator() == std::next(src.begin()));
        }
    }
}


static void test_errors() {
    enum match_id { INTEGER = 1 };
    enum error_id { SYNTAX_ERROR = 2 };

    const auto digit = range('0', '9');
    const auto integer = (+digit)->*INTEGER | error(SYNTAX_ERROR, digit);
    const auto grammar = *integer;

    {
        std::string src = "123+456-789";
        parse_context<> pc(src);
        const bool result = grammar.parse(pc);
        assert(result);

        assert(pc.parse_ended());
        
        assert(pc.matches().size() == 3);

        assert(pc.matches()[0].id() == INTEGER);
        assert(pc.matches()[0].start_position().iterator() == src.begin());
        assert(pc.matches()[0].end_iterator() == std::next(src.begin(), 3));
        assert(pc.matches()[0].source() == "123");

        assert(pc.matches()[1].id() == INTEGER);
        assert(pc.matches()[1].start_position().iterator() == std::next(src.begin(), 4));
        assert(pc.matches()[1].end_iterator() == std::next(src.begin(), 7));
        assert(pc.matches()[1].source() == "456");

        assert(pc.matches()[2].id() == INTEGER);
        assert(pc.matches()[2].start_position().iterator() == std::next(src.begin(), 8));
        assert(pc.matches()[2].end_iterator() == std::next(src.begin(), 11));
        assert(pc.matches()[2].source() == "789");

        assert(pc.errors().size() == 2);

        assert(pc.errors()[0].id() == SYNTAX_ERROR);
        assert(pc.errors()[0].start_position().iterator() == std::next(src.begin(), 3));
        assert(pc.errors()[0].end_iterator() == std::next(src.begin(), 4));
        assert(pc.errors()[0].source() == "+");

        assert(pc.errors()[1].id() == SYNTAX_ERROR);
        assert(pc.errors()[1].start_position().iterator() == std::next(src.begin(), 7));
        assert(pc.errors()[1].end_iterator() == std::next(src.begin(), 8));
        assert(pc.errors()[1].source() == "-");
    }
}


static void test_ast() {
    enum match_id { A, B, C, D, E, DE, PRG };

    const auto a = terminal('a')->*A;
    const auto b = terminal('b')->*B;
    const auto c = terminal('c')->*C;
    const auto d = terminal('d')->*D;
    const auto e = terminal('e')->*E;
    const auto de = (d >> e)->*DE;
    const auto term = a | b | c | de;
    const auto grammar = (*term)->*PRG;

    {
        std::string src = "abdec";
        parse_context<> pc(src);
        const bool result = grammar.parse(pc);
        assert(result);
        
        assert(pc.matches().size() == 1);
        auto ast = make_ast(pc.matches()[0]);

        assert(ast->id() == PRG);
        assert(ast->children().size() == 4);

        assert(ast->children()[0]->id() == A);
        assert(ast->children()[1]->id() == B);
        assert(ast->children()[2]->id() == DE);
        assert(ast->children()[3]->id() == C);

        assert(ast->children()[2]->children()[0]->id() == D);
        assert(ast->children()[2]->children()[1]->id() == E);
    }
}


void run_tests() {
    test_symbol_parsing();
    test_string_parsing();
    test_set_parsing();
    test_range_parsing();
    test_any_parsing();
    test_bool_parsing();
    test_end_parsing();
    test_function_parsing();
    test_newline_parsing();
    test_loop_0_parsing();
    test_loop_1_parsing();
    test_optional_parsing();
    test_logical_and_parsing();
    test_logical_not_parsing();
    test_sequence_parsing();
    test_choice_parsing();
    test_match_parsing();
    test_rule_parsing();
    calculator().test_rule_left_recursion_parsing();
    test_case_insensitive_parsing();
    test_non_character_parsing();
    #ifndef NDEBUG
    test_debug_annotations();
    #endif
    test_rule_optimizations();
    test_errors();
    test_ast();
}
