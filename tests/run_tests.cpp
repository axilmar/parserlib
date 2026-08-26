#include <sstream>
#include "parserlib.hpp"


using namespace parserlib;


using p = parser<>;


static void test_parse_symbol() {
    auto grammar = p::term('a');

    {
        std::string str = "a";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.end());
    }

    {
        std::string str = "b";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == str.begin());
    }
}


static void test_parse_string() {
    auto grammar = p::term("abc");

    {
        std::string str = "abc";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.end());
    }

    {
        std::string str = "abd";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == str.begin());
    }
}


static void test_parse_set() {
    auto grammar = p::set("0123456789");

    {
        std::string str = "0";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.end());
    }

    {
        std::string str = "5";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.end());
    }

    {
        std::string str = "9";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.end());
    }

    {
        std::string str = "a";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == str.begin());
    }
}


static void test_parse_range() {
    auto grammar = p::range('0', '9');

    {
        std::string str = "0";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.end());
    }

    {
        std::string str = "5";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.end());
    }

    {
        std::string str = "9";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.end());
    }

    {
        std::string str = "a";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == str.begin());
    }
}


static void test_parse_loop_0() {
    auto grammar = *p::term('a');

    {
        std::string str = "a";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.end());
    }

    {
        std::string str = "aa";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.end());
    }

    {
        std::string str = "";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.end());
    }

    {
        std::string str = "b";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.begin());
    }
}


static void test_parse_loop_1() {
    auto grammar = +p::term('a');

    {
        std::string str = "a";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.end());
    }

    {
        std::string str = "aa";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.end());
    }

    {
        std::string str = "";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == str.begin());
    }

    {
        std::string str = "b";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == str.begin());
    }
}


static void test_parse_loop_n() {
    auto grammar = p::term('a') * 2;

    {
        std::string str = "aa";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.end());
    }

    {
        std::string str = "";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == str.end());
    }

    {
        std::string str = "a";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == str.begin());
    }

    {
        std::string str = "b";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == str.begin());
    }
}


static void test_parse_optional() {
    auto grammar = -p::term('a');

    {
        std::string str = "a";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.end());
    }

    {
        std::string str = "b";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.begin());
    }
}


static void test_parse_logical_and() {
    auto grammar = &p::term('a');

    {
        std::string str = "a";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.begin());
    }

    {
        std::string str = "b";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == str.begin());
    }
}


static void test_parse_logical_not() {
    auto grammar = !p::term('a');

    {
        std::string str = "a";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == str.begin());
    }

    {
        std::string str = "b";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.begin());
    }
}


static void test_parse_sequence() {
    auto grammar = p::term('a') >> 'b' >> 'c';

    {
        std::string str = "abc";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.end());
    }

    {
        std::string str = "abd";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == str.begin());
    }
}


static void test_parse_choice() {
    auto grammar = p::term('a') | 'b' | 'c';

    {
        std::string str = "a";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.end());
    }

    {
        std::string str = "b";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.end());
    }

    {
        std::string str = "c";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.end());
    }

    {
        std::string str = "d";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == str.begin());
    }
}


static void test_parse_match() {
    {
        auto grammar = p::term("abc") ->* 1;

        {
            std::string str = "abc";
            p::parse_context pc{ str };
            bool ok = grammar.parse(pc);
            assert(ok);
            assert(pc.get_iterator() == str.end());
            assert(pc.get_matches().size() == 1);
            assert(pc.get_matches()[0].get_source() == "abc");
        }

        {
            std::string str = "abd";
            p::parse_context pc{ str };
            bool ok = grammar.parse(pc);
            assert(!ok);
            assert(pc.get_iterator() == str.begin());
            assert(pc.get_matches().size() == 0);
        }
    }

    {
        auto a = p::term('a') ->* 1;
        auto b = p::term('b') ->* 2;
        auto c = (a >> b) ->* 3;
        auto grammar = *(c | a | b);

        {
            std::string str = "abaabb";
            p::parse_context pc{ str };
            bool ok = grammar.parse(pc);
            assert(ok);
            assert(pc.get_iterator() == str.end());
            assert(pc.get_matches().size() == 4);
            assert(pc.get_matches()[0].get_source() == "ab");
            assert(pc.get_matches()[0].get_children().size() == 2);
            assert(pc.get_matches()[0].get_children()[0].get_source() == "a");
            assert(pc.get_matches()[0].get_children()[1].get_source() == "b");
            assert(pc.get_matches()[1].get_source() == "a");
            assert(pc.get_matches()[2].get_source() == "ab");
            assert(pc.get_matches()[2].get_children().size() == 2);
            assert(pc.get_matches()[2].get_children()[0].get_source() == "a");
            assert(pc.get_matches()[2].get_children()[1].get_source() == "b");
            assert(pc.get_matches()[3].get_source() == "b");
        }
    }
}


static void test_parse_explicit_match() {
    {
        auto grammar = p::begin_match() >> p::term("abc") >> p::add_match(1) >> p::end_match();

        {
            std::string str = "abc";
            p::parse_context pc{ str };
            bool ok = grammar.parse(pc);
            assert(ok);
            assert(pc.get_iterator() == str.end());
            assert(pc.get_matches().size() == 1);
            assert(pc.get_matches()[0].get_source() == "abc");
        }

        {
            std::string str = "abd";
            p::parse_context pc{ str };
            bool ok = grammar.parse(pc);
            assert(!ok);
            assert(pc.get_iterator() == str.begin());
            assert(pc.get_matches().size() == 0);
        }
    }

    {
        auto a = p::begin_match() >> p::term('a') >> p::add_match(1) >> p::end_match();
        auto b = p::begin_match() >> p::term('b') >> p::add_match(2) >> p::end_match();
        auto c = p::begin_match() >> a >> b >> p::add_match(3) >> p::end_match();
        auto grammar = *(c | a | b);

        {
            std::string str = "abaabb";
            p::parse_context pc{ str };
            bool ok = grammar.parse(pc);
            assert(ok);
            assert(pc.get_iterator() == str.end());
            assert(pc.get_matches().size() == 4);
            assert(pc.get_matches()[0].get_source() == "ab");
            assert(pc.get_matches()[0].get_children().size() == 2);
            assert(pc.get_matches()[0].get_children()[0].get_source() == "a");
            assert(pc.get_matches()[0].get_children()[1].get_source() == "b");
            assert(pc.get_matches()[1].get_source() == "a");
            assert(pc.get_matches()[2].get_source() == "ab");
            assert(pc.get_matches()[2].get_children().size() == 2);
            assert(pc.get_matches()[2].get_children()[0].get_source() == "a");
            assert(pc.get_matches()[2].get_children()[1].get_source() == "b");
            assert(pc.get_matches()[3].get_source() == "b");
        }
    }
}


static void test_parse_any() {
    auto grammar = p::any();

    {
        std::string str = "a";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.end());
    }

    {
        std::string str = "ab";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == std::next(str.begin()));
    }

    {
        std::string str = "";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == str.end());
    }
}


static void test_parse_end() {
    auto grammar = p::end();

    {
        std::string str = "";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == str.end());
    }

    {
        std::string str = "a";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == str.begin());
    }
}


static void test_parse_newline() {
    using p = parser<text_position_iterator<>>;
    auto grammar = *(p::term('a')->*1 | p::term('b')->*2 | p::term('c')->*3 | p::newline(p::term('\n')));

    {
        std::string str = "a\nb\nc\n";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_matches().size() == 3);

        assert(pc.get_matches()[0].get_source() == "a");
        assert(pc.get_matches()[0].begin().get_line() == 1);
        assert(pc.get_matches()[0].begin().get_column() == 1);
        assert(pc.get_matches()[0].end().get_line() == 1);
        assert(pc.get_matches()[0].end().get_column() == 2);

        assert(pc.get_matches()[1].get_source() == "b");
        assert(pc.get_matches()[1].begin().get_line() == 2);
        assert(pc.get_matches()[1].begin().get_column() == 1);
        assert(pc.get_matches()[1].end().get_line() == 2);
        assert(pc.get_matches()[1].end().get_column() == 2);

        assert(pc.get_matches()[2].get_source() == "c");
        assert(pc.get_matches()[2].begin().get_line() == 3);
        assert(pc.get_matches()[2].begin().get_column() == 1);
        assert(pc.get_matches()[2].end().get_line() == 3);
        assert(pc.get_matches()[2].end().get_column() == 2);
    }
}


static void test_parse_function() {
    bool parsed = false;

    auto grammar = p::function([&](p::parse_context& pc) {
        parsed = true;
        return true;
    });

    std::string str;
    p::parse_context pc{ str };
    bool ok = grammar.parse(pc);
    assert(ok);
    assert(parsed == true);
}


static void test_parse_error() {
    using p = parser<text_position_iterator<>>;

    {
        auto grammar = *(p::term('a') >> ';' | p::error(p::skip_after(';'), 1));

        {
            std::string str = "a;a;a;";
            p::parse_context pc{ str };
            bool ok = grammar.parse(pc);
            assert(ok);
            assert(pc.get_errors().size() == 0);
        }

        {
            std::string str = "a;b;a;";
            p::parse_context pc{ str };
            bool ok = grammar.parse(pc);
            assert(ok);
            assert(pc.get_errors().size() == 1);
            assert(pc.get_errors()[0].begin().get_column() == 3);
            assert(pc.get_errors()[0].end().get_column() == 5);
        }
    }

    {
        auto grammar = *((p::term('a') | p::error(p::skip_before(';'), 1)) >> ';');

        {
            std::string str = "a;a;a;";
            p::parse_context pc{ str };
            bool ok = grammar.parse(pc);
            assert(ok);
            assert(pc.get_errors().size() == 0);
        }

        {
            std::string str = "a;b;a;";
            p::parse_context pc{ str };
            bool ok = grammar.parse(pc);
            assert(ok);
            assert(pc.get_errors().size() == 1);
            assert(pc.get_errors()[0].begin().get_column() == 3);
            assert(pc.get_errors()[0].end().get_column() == 4);
        }
    }
}


static void test_parse_rule() {
    {
        p::rule grammar = 'a';
        std::string str = "a";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
    }

    {
        p::rule grammar = "abc";
        std::string str = "abc";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
    }

    {
        p::rule grammar = p::set("0123456789");
        std::string str = "0";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
    }

    {
        p::rule grammar = p::range('0', '9');
        std::string str = "0";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
    }

    {
        p::rule grammar = *p::term('a');
        std::string str = "aa";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
    }

    {
        p::rule grammar = +p::term('a');
        std::string str = "aa";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
    }

    {
        p::rule grammar = -p::term('a');
        std::string str = "a";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
    }

    {
        p::rule grammar = &p::term('a');
        std::string str = "a";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
    }

    {
        p::rule grammar = !p::term('a');
        std::string str = "a";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(!ok);
    }

    {
        p::rule a = 'a';
        p::rule b = 'b';
        p::rule grammar = a >> b >> 'c';
        std::string str = "abc";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
    }

    {
        p::rule a = 'a';
        p::rule b = 'b';
        p::rule grammar = a | b | 'c';
        std::string str = "a";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
    }

    {
        p::rule a = 'a';
        p::rule grammar = a ->* 1;
        std::string str = "a";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
    }

    {
        p::rule grammar = p::term('a') >> grammar | p::end();
        std::string str = "aa";
        p::parse_context pc{ str };
        bool ok = grammar.parse(pc);
        assert(ok);
    }
}


class calculator {
public:
    enum ID {
        NUM,
        ADD,
        SUB,
        MUL,
        DIV
    };

    calculator() {
        auto digit
            = p::range('0', '9')
            ;

        auto sign
            = p::term('+') | '-'
            ;

        auto integer
            = +digit
            ;

        auto number
            = (-sign >> integer >> -('.' >> integer)) ->* NUM
            ;

        auto val
            = number
            | '(' >> m_expression >> ')'
            ;

        auto mul
            = p::left_associative(
                val,
                '*' >> val >> p::match(MUL),
                '/' >> val >> p::match(DIV)
            );

        auto add
            = p::left_associative(
                mul,
                '+' >> mul >> p::match(ADD),
                '-' >> mul >> p::match(SUB)
            );

        m_expression = add;
    }

    bool parse(p::parse_context& pc) const {
        return m_expression.parse(pc);
    }

    double evaluate(const p::match_instance& match) {
        switch (match.get_id()) {
            case NUM: {
                std::stringstream stream;
                stream << match.get_source();
                double result;
                stream >> result;
                return result;
            }

            case ADD:
                assert(match.get_children().size() == 2);
                return evaluate(match.get_children()[0]) + evaluate(match.get_children()[1]);

            case SUB:
                assert(match.get_children().size() == 2);
                return evaluate(match.get_children()[0]) - evaluate(match.get_children()[1]);

            case MUL:
                assert(match.get_children().size() == 2);
                return evaluate(match.get_children()[0]) * evaluate(match.get_children()[1]);

            case DIV:
                assert(match.get_children().size() == 2);
                return evaluate(match.get_children()[0]) / evaluate(match.get_children()[1]);
        }

        throw std::logic_error("invalid operation");
    }

private:
    p::rule m_expression;
};


#define TEST_CALC(EXPR) test_calc(calc, EXPR, #EXPR)


static void test_calc(calculator& calc, double expected_result, const std::string& expr) {
    p::parse_context pc{ expr };
    bool ok = calc.parse(pc);
    assert(ok);
    assert(pc.get_matches().size() == 1);
    double parsed_result = calc.evaluate(pc.get_matches()[0]);
    assert(parsed_result == expected_result);
}


static void test_parse_left_associative() {
    calculator calc;
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
    test_parse_explicit_match();
    test_parse_any();
    test_parse_end();
    test_parse_newline();
    test_parse_function();
    test_parse_error();
    test_parse_rule();
    test_parse_left_associative();
}
