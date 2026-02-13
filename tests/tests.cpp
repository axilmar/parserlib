#include <functional>
#include <sstream>
#include <chrono>
#include <iostream>
#include "parserlib.hpp"
using namespace parserlib;


using default_parse_context = parse_context<>;
using p = parser<>;
using p2 = parser<parse_context<default_parse_context::match_container_type::const_iterator>>;
using p3 = parser<parse_context<source_file_iterator<>>>;


static void test_parse_symbol() {
    const auto grammar = p::terminal('a');

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_iterator());
    }

    {
        std::string source = "b";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.is_valid_iterator());
    }
}


static void test_parse_string() {
    const auto grammar = p::terminal("abc");

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
    const auto grammar = p::set("0123456789");

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


static void test_parse_range() {
    const auto grammar = p::range('0', '9');

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


static void test_parse_loop_0() {
    const auto grammar = *p::terminal('a');

    {
        std::string source = "";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_iterator());
    }

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_iterator());
    }

    {
        std::string source = "aa";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_iterator());
    }

    {
        std::string source = "aaa";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_iterator());
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


static void test_parse_loop_1() {
    const auto grammar = +p::terminal('a');

    {
        std::string source = "";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.is_end_iterator());
    }

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_iterator());
    }

    {
        std::string source = "aa";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_iterator());
    }

    {
        std::string source = "aaa";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_iterator());
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


static void test_parse_loop_n() {
    const auto grammar = 4 * p::terminal('a');

    {
        std::string source = "aaaa";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_iterator());
    }

    {
        std::string source = "aaa";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.get_iterator() == source.begin());
    }

    {
        std::string source = "aaaaa";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == std::next(source.begin(), 4));
    }
}


static void test_parse_optional() {
    const auto grammar = -p::terminal('a');

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


static void test_parse_logical_and() {
    const auto grammar = &p::terminal('a');

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_valid_iterator());
    }

    {
        std::string source = "b";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.is_valid_iterator());
    }
}


static void test_parse_logical_not() {
    const auto grammar = !p::terminal('b');

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_valid_iterator());
    }

    {
        std::string source = "b";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.is_valid_iterator());
    }
}


static void test_parse_sequence() {
    const auto grammar = p::terminal('a') >> "b" >> 'c';

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


static void test_parse_choice() {
    const auto grammar = p::terminal('a') | 'b' | 'c';

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_iterator());
    }

    {
        std::string source = "b";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_iterator());
    }

    {
        std::string source = "c";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_iterator());
    }

    {
        std::string source = "d";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.is_valid_iterator());
    }
}


static void test_parse_match() {
    enum MATCH_ID { A, B, C, D };

    const auto a = p::terminal('a')->*A;
    const auto b = p::terminal('b')->*B;
    const auto c = p::terminal('c')->*C;
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


static void test_parse_matches() {
    enum { A, B, C };

    const auto a = p::terminal('a')->*A;
    const auto b = p::terminal('b')->*B;
    const auto c = p::terminal('c')->*C;
    const auto grammar1 = a >> b >> c;

    std::string source = "abc";
    parse_context<> pc1(source);
    const auto result1 = grammar1.parse(pc1);
    assert(result1);
    assert(pc1.get_matches().size() == 3);

    auto pc2 = pc1.derive_parse_context();
    const auto grammar2 = (p2::terminal(A)->*A) >> (p2::terminal(B)->*B) >> (p2::terminal(C)->*C);
    const auto result2 = grammar2.parse(pc2);
    assert(result2);
    assert(pc2.get_matches().size() == pc1.get_matches().size());
    assert(pc2.get_matches()[0].get_id() == pc1.get_matches()[0].get_id());
    assert(pc2.get_matches()[1].get_id() == pc1.get_matches()[1].get_id());
    assert(pc2.get_matches()[2].get_id() == pc1.get_matches()[2].get_id());
}


static void test_parse_any() {
    const auto grammar = p::any();

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_iterator());
    }

    {
        std::string source = "";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.is_end_iterator());
    }
}


static void test_parse_end() {
    const auto grammar = p::end();

    {
        std::string source = "";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_iterator());
    }

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.is_valid_iterator());
    }
}


static void test_parse_bool() {
    {
        const auto grammar = p::make_parse_node(true);
        std::string source = "";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_iterator());
    }

    {
        const auto grammar = p::make_parse_node(false);
        std::string source = "";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.is_end_iterator());
    }
}


static void test_parse_newline() {
    enum { A };

    const auto grammar = *(p3::newline('\n') | p3::terminal('a')->*A);

    {
        std::string source = "a\na";
        parse_context<source_file_iterator<>> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator().get_line() == 2);
        assert(pc.get_iterator().get_column() == 2);
        assert(pc.get_matches().size() == 2);
        assert(pc.get_matches()[0].get_id() == A);
        assert(pc.get_matches()[1].get_id() == A);
    }
}


static void test_parse_function() {
    const auto grammar = p::function([](auto& pc) {
        if (pc.is_valid_iterator() && pc.compare(*pc.get_iterator(), 'a') == 0) {
            pc.increment_iterator();
            return true;
        }
        return false;
    });

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_iterator());
    }

    {
        std::string source = "b";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.is_valid_iterator());
    }
}


static void test_parse_rule() {
    using parse_context_type = parse_context<>;

    p::rule grammar = p::terminal('a');

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


static void test_parse_rule_recursion() {
    using parse_context_type = parse_context<>;

    const p::rule grammar = -(p::terminal('a') >> grammar);

    {
        std::string source = "";
        parse_context_type pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_iterator());
    }

    {
        std::string source = "a";
        parse_context_type pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_iterator());
    }

    {
        std::string source = "aa";
        parse_context_type pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_iterator());
    }

    {
        std::string source = "aaa";
        parse_context_type pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_iterator());
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


static void test_parse_rule_left_recursion() {
    /**** types ****/

    using parse_context_type = parse_context<>;

    using match_type = typename parse_context_type::match_type;

    using rule_type = p::rule;

    /**** grammar ****/

    enum {NUM, ADD, SUB, MUL, DIV};

    rule_type add, mul;

    auto digit = p::range('0', '9');

    auto num = -p::terminal('-') >> (+digit >> -('.' >> +digit))->*NUM;

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


static void test_parse_case_sensitive() {
    using p = parser<parse_context<std::string::const_iterator, int, int, case_insensitive_symbol_comparator>>;

    const auto grammar = p::terminal('a');

    {
        std::string source = "a";
        parse_context<std::string::const_iterator, int, int, case_insensitive_symbol_comparator> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
    }

    {
        std::string source = "A";
        parse_context<std::string::const_iterator, int, int, case_insensitive_symbol_comparator> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
    }

    {
        std::string source = "b";
        parse_context<std::string::const_iterator, int, int, case_insensitive_symbol_comparator> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
    }

    {
        std::string source = "B";
        parse_context<std::string::const_iterator, int, int, case_insensitive_symbol_comparator> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
    }
}


//static void test_ast() {
//    enum { GRAMMAR, A, B, C };
//
//    const auto a = p::terminal('a')->*A;
//    const auto b = p::terminal('b')->*B;
//    const auto c = p::terminal('c')->*C;
//    const auto grammar = (a >> b >> c)->*GRAMMAR;
//
//    std::string source = "abc";
//    parse_context<> pc1(source);
//    const auto result1 = grammar.parse(pc1);
//    assert(result1);
//    assert(pc1.get_matches().size() == 1);
//    auto ast = make_ast_node(pc1.get_matches()[0]);
//
//    assert(ast->get_id() == GRAMMAR);
//    assert((*std::next(ast->get_children().begin(), 0))->get_id() == A);
//    assert((*std::next(ast->get_children().begin(), 1))->get_id() == B);
//    assert((*std::next(ast->get_children().begin(), 2))->get_id() == C);
//}


void run_tests() {
    test_parse_symbol();
    test_parse_string();
    test_parse_set();
    test_parse_range();
    test_parse_loop_0();
    test_parse_loop_1();
    test_parse_loop_n();
    test_parse_optional();
    test_parse_logical_and();
    test_parse_logical_not();
    test_parse_sequence();
    test_parse_choice();
    test_parse_match();
    test_parse_matches();
    test_parse_any();
    test_parse_end();
    test_parse_bool();
    test_parse_newline();
    test_parse_function();
    test_parse_rule();
    test_parse_rule_recursion();
    //test_parse_rule_left_recursion();
    test_parse_case_sensitive();
    //test_ast();
}
