#include <functional>
#include <sstream>
#include <chrono>
#include <iostream>
#include "parserlib.hpp"
using namespace parserlib;


static void test_parse_any() {
    const auto grammar = any();

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.is_end_parse_position());
    }
}


static void test_parse_bool() {
    {
        const auto grammar = make_parse_node(true);
        std::string source = "";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = make_parse_node(false);
        std::string source = "";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.is_end_parse_position());
    }
}


static void test_parse_case_sensitive() {
    const auto grammar = terminal('a');

    {
        std::string source = "a";
        parse_context<std::string::const_iterator, int, int, case_sensitive_symbol_comparator> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
    }

    {
        std::string source = "A";
        parse_context<std::string::const_iterator, int, int, case_sensitive_symbol_comparator> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
    }

    {
        std::string source = "b";
        parse_context<std::string::const_iterator, int, int, case_sensitive_symbol_comparator> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
    }

    {
        std::string source = "B";
        parse_context<std::string::const_iterator, int, int, case_sensitive_symbol_comparator> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
    }
}


static void test_parse_choice() {
    const auto grammar = terminal('a') | 'b' | 'c';

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "b";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "c";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "d";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.is_valid_parse_position());
    }
}


static void test_parse_end() {
    const auto grammar = end();

    {
        std::string source = "";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.is_valid_parse_position());
    }
}


static void test_parse_error_skip_before() {
    const auto grammar = terminal('a') >> terminal(';') | error(1, skip_before(';'));

    {
        std::string source = "a;";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_errors().size() == 0);
    }

    {
        std::string source = "b;";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == std::next(source.begin(), 1));
        assert(pc.get_errors().size() == 1);
        assert(pc.get_errors()[0].begin() == std::next(source.begin(), 0));
        assert(pc.get_errors()[0].end() == std::next(source.begin(), 1));
    }
}


static void test_parse_error_skip_after() {
    const auto grammar = terminal('a') >> terminal(';') | error(1, skip_after(';'));

    {
        std::string source = "a;";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_errors().size() == 0);
    }

    {
        std::string source = "b;";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == std::next(source.begin(), 2));
        assert(pc.get_errors().size() == 1);
        assert(pc.get_errors()[0].begin() == std::next(source.begin(), 0));
        assert(pc.get_errors()[0].end() == std::next(source.begin(), 2));
    }
}


static void test_parse_function() {
    const auto grammar = function([](auto& pc) {
        if (pc.is_valid_parse_position() && pc.compare_symbols(pc.get_symbol(), 'a') == 0) {
            pc.increment_parse_position();
            return true;
        }
        return false;
    });

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "b";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.is_valid_parse_position());
    }
}


static void test_parse_logical_and() {
    const auto grammar = &terminal('a');

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_valid_parse_position());
    }

    {
        std::string source = "b";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.is_valid_parse_position());
    }
}


static void test_parse_logical_not() {
    const auto grammar = !terminal('b');

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_valid_parse_position());
    }

    {
        std::string source = "b";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.is_valid_parse_position());
    }
}


static void test_parse_loop0() {
    const auto grammar = *terminal('a');

    {
        std::string source = "";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "aa";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "aaa";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "b";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == source.begin());
    }

    {
        std::string source = "ab";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == std::next(source.begin(), 1));
    }

    {
        std::string source = "aab";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == std::next(source.begin(), 2));
    }
}


static void test_parse_loop1() {
    const auto grammar = +terminal('a');

    {
        std::string source = "";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "aa";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "aaa";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "b";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.get_iterator() == source.begin());
    }

    {
        std::string source = "ab";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == std::next(source.begin(), 1));
    }

    {
        std::string source = "aab";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == std::next(source.begin(), 2));
    }
}


static void test_parse_match() {
    enum MATCH_ID { A, B, C, D };

    const auto a = terminal('a')->*A;
    const auto b = terminal('b')->*B;
    const auto c = terminal('c')->*C;
    const auto d = (b >> c)->*D;
    const auto grammar = a | d | b | c;

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_matches().size() == 1);
        assert(pc.get_matches()[0].get_id() == A);
        assert(pc.get_matches()[0].begin() == std::next(source.begin(), 0));
        assert(pc.get_matches()[0].end() == std::next(source.begin(), 1));
    }

    {
        std::string source = "b";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_matches().size() == 1);
        assert(pc.get_matches()[0].get_id() == B);
        assert(pc.get_matches()[0].begin() == std::next(source.begin(), 0));
        assert(pc.get_matches()[0].end() == std::next(source.begin(), 1));
    }

    {
        std::string source = "c";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_matches().size() == 1);
        assert(pc.get_matches()[0].get_id() == C);
        assert(pc.get_matches()[0].begin() == std::next(source.begin(), 0));
        assert(pc.get_matches()[0].end() == std::next(source.begin(), 1));
    }

    {
        std::string source = "bc";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_matches().size() == 1);
        assert(pc.get_matches()[0].get_id() == D);
        assert(pc.get_matches()[0].begin() == std::next(source.begin(), 0));
        assert(pc.get_matches()[0].end() == std::next(source.begin(), 2));
        assert(pc.get_matches()[0].get_children().size() == 2);
        assert(pc.get_matches()[0].get_children()[0].get_id() == B);
        assert(pc.get_matches()[0].get_children()[0].begin() == std::next(source.begin(), 0));
        assert(pc.get_matches()[0].get_children()[0].end() == std::next(source.begin(), 1));
        assert(pc.get_matches()[0].get_children()[1].get_id() == C);
        assert(pc.get_matches()[0].get_children()[1].begin() == std::next(source.begin(), 1));
        assert(pc.get_matches()[0].get_children()[1].end() == std::next(source.begin(), 2));
    }
}


static void test_parse_newline() {
    enum { A };

    const auto grammar = *(newline('\n') | terminal('a')->*A);

    {
        std::string source = "a\na";
        parse_context<parse_iterator<std::string::const_iterator, file_text_position>> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator().get_text_position().get_line() == 2);
        assert(pc.get_iterator().get_text_position().get_column() == 2);
        assert(pc.get_matches().size() == 2);
        assert(pc.get_matches()[0].get_id() == A);
        assert(pc.get_matches()[1].get_id() == A);
        assert(pc.get_matches()[0].get_source() == "a");
        assert(pc.get_matches()[1].get_source() == "a");
    }
}


static void test_parse_optional() {
    const auto grammar = -terminal('a');

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == source.end());
    }

    {
        std::string source = "b";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == source.begin());
    }
}


static void test_parse_node_ptr() {
    using parse_context_type = parse_context<>;

    parse_node_ptr<parse_context_type> grammar = terminal('a');

    {
        std::string source = "a";
        parse_context_type pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
    }
}


static void test_parse_range() {
    const auto grammar = range('0', '9');

    {
        std::string source = "0";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == source.end());
    }

    {
        std::string source = "5";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == source.end());
    }

    {
        std::string source = "9";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == source.end());
    }

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.get_iterator() == source.begin());
    }
}


static void test_parse_rule() {
    using parse_context_type = parse_context<>;

    rule grammar = terminal('a');

    {
        std::string source = "a";
        parse_context_type pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == source.end());
    }

    {
        std::string source = "b";
        parse_context_type pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.get_iterator() == source.begin());
    }
}


static void test_parse_sequence() {
    const auto grammar = terminal('a') >> "b" >> 'c';

    {
        std::string source = "abc";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == source.end());
    }

    {
        std::string source = "xbc";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.get_iterator() == source.begin());
    }

    {
        std::string source = "axc";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.get_iterator() == source.begin());
    }

    {
        std::string source = "abx";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.get_iterator() == source.begin());
    }
}


static void test_parse_set() {
    const auto grammar = set("0123456789");

    {
        std::string source = "0";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == source.end());
    }

    {
        std::string source = "5";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == source.end());
    }

    {
        std::string source = "9";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == source.end());
    }

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.get_iterator() == source.begin());
    }
}


static void test_parse_string() {
    const auto grammar = terminal("abc");

    {
        std::string source = "abc";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == source.end());
    }

    {
        std::string source = "xbc";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.get_iterator() == source.begin());
    }

    {
        std::string source = "axc";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.get_iterator() == source.begin());
    }

    {
        std::string source = "abx";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.get_iterator() == source.begin());
    }
}


static void test_parse_symbol() {
    const auto grammar = terminal('a');

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "b";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.is_valid_parse_position());
    }
}


static void test_parse_recursion() {
    using parse_context_type = parse_context<>;

    const rule<parse_context_type> grammar = -(terminal('a') >> grammar);

    {
        std::string source = "";
        parse_context_type pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "a";
        parse_context_type pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "aa";
        parse_context_type pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "aaa";
        parse_context_type pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "b";
        parse_context_type pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == source.begin());
    }

    {
        std::string source = "ab";
        parse_context_type pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == std::next(source.begin(), 1));
    }

    {
        std::string source = "aab";
        parse_context_type pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == std::next(source.begin(), 2));
    }
}


static void test_parse_left_recursion() {
    /**** types ****/

    using parse_context_type = parse_context<>;

    using match_type = typename parse_context_type::match_type;

    using rule_type = rule<parse_context_type>;

    /**** grammar ****/

    enum {NUM, ADD, SUB, MUL, DIV};

    rule_type add, mul;

    auto digit = range('0', '9');

    auto num = -terminal('-') >> (+digit >> -('.' >> +digit))->*NUM;

    auto val = '(' >> add >> ')'
             | num;

    add = (add >> '+' >> mul)->*ADD
        | (add >> '-' >> mul)->*SUB
        | mul;

    mul = (mul >> '*' >> val)->*MUL
        | (mul >> '/' >> val)->*DIV
        | val;

    auto grammar = add;

    /*** helper functions ****/

    std::function<double(const match_type&)> eval;

    eval = [&](const match_type& match) {
        switch (match.get_id()) {
            case NUM: {
                std::stringstream stream;
                stream << match.get_source();
                double v;
                stream >> v;
                return v;
            }

            case ADD:
                assert(match.get_children().size() == 2);
                return eval(match.get_children()[0]) + eval(match.get_children()[1]);

            case SUB:
                assert(match.get_children().size() == 2);
                return eval(match.get_children()[0]) - eval(match.get_children()[1]);

            case MUL:
                assert(match.get_children().size() == 2);
                return eval(match.get_children()[0]) * eval(match.get_children()[1]);

            case DIV:
                assert(match.get_children().size() == 2);
                return eval(match.get_children()[0]) / eval(match.get_children()[1]);
        }

        throw std::runtime_error("calculator::eval: invalid match id");
    };

    auto calc = [&](const char* expr, double val) {
        std::string source = expr;
        parse_context_type pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_matches().size() == 1);
        const double eval_value = eval(pc.get_matches()[0]);
        assert(eval_value == val);
    };

    #define TEST_CALC(EXPR) calc(#EXPR, EXPR)

    /*** tests ****/

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


static void test_parse_matches() {
    enum { A, B, C };

    const auto a = terminal('a')->*A;
    const auto b = terminal('b')->*B;
    const auto c = terminal('c')->*C;
    const auto grammar1 = a >> b >> c;

    std::string source = "abc";
    parse_context<> pc1(source);
    const auto result1 = grammar1.parse(pc1);
    assert(result1);
    assert(pc1.get_matches().size() == 3);

    auto pc2 = pc1.derive_parse_context();
    const auto grammar2 = (terminal(A)->*A) >> (terminal(B)->*B) >> (terminal(C)->*C);
    const auto result2 = grammar2.parse(pc2);
    assert(result2);
    assert(pc2.get_matches().size() == pc1.get_matches().size());
    assert(pc2.get_matches()[0].get_id() == pc1.get_matches()[0].get_id());
    assert(pc2.get_matches()[1].get_id() == pc1.get_matches()[1].get_id());
    assert(pc2.get_matches()[2].get_id() == pc1.get_matches()[2].get_id());
}


static void test_ast() {
    enum { GRAMMAR, A, B, C };

    const auto a = terminal('a')->*A;
    const auto b = terminal('b')->*B;
    const auto c = terminal('c')->*C;
    const auto grammar = (a >> b >> c)->*GRAMMAR;

    std::string source = "abc";
    parse_context<> pc1(source);
    const auto result1 = grammar.parse(pc1);
    assert(result1);
    assert(pc1.get_matches().size() == 1);
    auto ast = make_ast_node(pc1.get_matches()[0]);

    assert(ast->get_id() == GRAMMAR);
    assert((*std::next(ast->get_children().begin(), 0))->get_id() == A);
    assert((*std::next(ast->get_children().begin(), 1))->get_id() == B);
    assert((*std::next(ast->get_children().begin(), 2))->get_id() == C);
}


static void test_generic_iterator() {
    const auto grammar = terminal('a');

    using iterator_type = generic_iterator;

    {
        std::string source = "a";
        parse_context<iterator_type> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "b";
        parse_context<iterator_type> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.is_valid_parse_position());
    }
}


#include "parserlib/tuple.hpp"


static void test_tuple() {
    {
        auto t = make_tuple();
        assert(tuple_size_v<decltype(t)> == 0);
    }

    {
        auto t = make_tuple(1);
        assert(tuple_size_v<decltype(t)> == 1);
        assert(get<0>(t) == 1);
    }

    {
        auto t = make_tuple(1, 'a');
        assert(tuple_size_v<decltype(t)> == 2);
        assert(get<0>(t) == 1);
        assert(get<1>(t) == 'a');
    }

    {
        auto t = make_tuple(1, 'a', 3.14);
        assert(tuple_size_v<decltype(t)> == 3);
        assert(get<0>(t) == 1);
        assert(get<1>(t) == 'a');
        assert(get<2>(t) == 3.14);
    }

    {
        auto t = tuple_cat(make_tuple());
        assert(tuple_size_v<decltype(t)> == 0);
    }

    {
        auto t = tuple_cat(make_tuple(1), make_tuple());
        assert(tuple_size_v<decltype(t)> == 1);
        assert(get<0>(t) == 1);
    }

    {
        auto t = tuple_cat(make_tuple(), make_tuple(1));
        assert(tuple_size_v<decltype(t)> == 1);
        assert(get<0>(t) == 1);
    }

    {
        auto t = tuple_cat(make_tuple(1), make_tuple(2));
        assert(tuple_size_v<decltype(t)> == 2);
        assert(get<0>(t) == 1);
        assert(get<1>(t) == 2);
    }

    {
        auto t = tuple_cat(make_tuple(1, 2), make_tuple(3));
        assert(tuple_size_v<decltype(t)> == 3);
        assert(get<0>(t) == 1);
        assert(get<1>(t) == 2);
        assert(get<2>(t) == 3);
    }

    {
        auto t = tuple_cat(make_tuple(1), make_tuple(2, 3));
        assert(tuple_size_v<decltype(t)> == 3);
        assert(get<0>(t) == 1);
        assert(get<1>(t) == 2);
        assert(get<2>(t) == 3);
    }

    {
        auto t = tuple_cat(make_tuple(1, 2), make_tuple(3, 4));
        assert(tuple_size_v<decltype(t)> == 4);
        assert(get<0>(t) == 1);
        assert(get<1>(t) == 2);
        assert(get<2>(t) == 3);
        assert(get<3>(t) == 4);
    }

    {
        auto t = tuple_cat(make_tuple(1, 2, 3), make_tuple(4, 5));
        assert(tuple_size_v<decltype(t)> == 5);
        assert(get<0>(t) == 1);
        assert(get<1>(t) == 2);
        assert(get<2>(t) == 3);
        assert(get<3>(t) == 4);
        assert(get<4>(t) == 5);
    }

    {
        auto t = tuple_cat(make_tuple(1, 2), make_tuple(3, 4, 5));
        assert(tuple_size_v<decltype(t)> == 5);
        assert(get<0>(t) == 1);
        assert(get<1>(t) == 2);
        assert(get<2>(t) == 3);
        assert(get<3>(t) == 4);
        assert(get<4>(t) == 5);
    }

    {
        auto t = tuple_cat(make_tuple(1, 2, 3), make_tuple(4, 5, 6));
        assert(tuple_size_v<decltype(t)> == 6);
        assert(get<0>(t) == 1);
        assert(get<1>(t) == 2);
        assert(get<2>(t) == 3);
        assert(get<3>(t) == 4);
        assert(get<4>(t) == 5);
        assert(get<5>(t) == 6);
    }

    {
        auto t = tuple_cat(make_tuple(1, 2), make_tuple(3, 4), make_tuple(5, 6));
        assert(tuple_size_v<decltype(t)> == 6);
        assert(get<0>(t) == 1);
        assert(get<1>(t) == 2);
        assert(get<2>(t) == 3);
        assert(get<3>(t) == 4);
        assert(get<4>(t) == 5);
        assert(get<5>(t) == 6);
    }
}


void run_tests() {
    test_parse_any();
    test_parse_bool();
    test_parse_case_sensitive();
    test_parse_choice();
    test_parse_end();
    test_parse_error_skip_before();
    test_parse_error_skip_after();
    test_parse_function();
    test_parse_logical_and();
    test_parse_logical_not();
    test_parse_loop0();
    test_parse_loop1();
    test_parse_match();
    test_parse_newline();
    test_parse_optional();
    test_parse_node_ptr();
    test_parse_range();
    test_parse_rule();
    test_parse_sequence();
    test_parse_set();
    test_parse_string();
    test_parse_symbol();
    test_parse_recursion();
    test_parse_left_recursion();
    test_parse_matches();
    test_ast();
    test_generic_iterator();
    test_tuple();
}
