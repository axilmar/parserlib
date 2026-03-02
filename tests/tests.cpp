#include <cassert>
#include "parserlib.hpp"


using namespace parserlib;


using p = parser<>;


static void test_parse_symbol() {
    const auto grammar = p::terminal('a');

    {
        std::string src = "a";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    {
        std::string src = "b";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_string() {
    const auto grammar = p::terminal("abc");

    {
        std::string src = "abc";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    {
        std::string src = "abd";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_set() {
    const auto grammar = p::set("abc");

    {
        std::string src = "a";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    {
        std::string src = "b";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    {
        std::string src = "c";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    {
        std::string src = "d";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_range() {
    const auto grammar = p::range('0', '9');

    {
        std::string src = "0";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    {
        std::string src = "5";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    {
        std::string src = "9";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    {
        std::string src = "a";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_loop_0() {
    const auto grammar = *p::terminal('a');

    {
        std::string src = "";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    {
        std::string src = "a";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    {
        std::string src = "aa";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    {
        std::string src = "aab";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == std::prev(src.end()));
    }

    {
        std::string src = "b";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_loop_1() {
    const auto grammar = +p::terminal('a');

    {
        std::string src = "a";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    {
        std::string src = "aa";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    {
        std::string src = "aab";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == std::prev(src.end()));
    }

    {
        std::string src = "";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }

    {
        std::string src = "b";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_loop_n() {
    {
        const auto grammar = 4 * p::terminal('a');

        {
            std::string src = "aaaa";
            p::parse_context pc(src);
            const bool ok = grammar.parse(pc);
            assert(ok);
            assert(pc.get_iterator() == src.end());
        }

        {
            std::string src = "aaab";
            p::parse_context pc(src);
            const bool ok = grammar.parse(pc);
            assert(!ok);
            assert(pc.get_iterator() == src.begin());
        }
    }

    {
        const auto grammar = p::terminal('a') * 4;

        {
            std::string src = "aaaa";
            p::parse_context pc(src);
            const bool ok = grammar.parse(pc);
            assert(ok);
            assert(pc.get_iterator() == src.end());
        }

        {
            std::string src = "aaab";
            p::parse_context pc(src);
            const bool ok = grammar.parse(pc);
            assert(!ok);
            assert(pc.get_iterator() == src.begin());
        }
    }
}


static void test_parse_optional() {
    const auto grammar = -p::terminal('a');

    {
        std::string src = "a";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    {
        std::string src = "b";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_logical_and() {
    const auto grammar = &p::terminal('a');

    {
        std::string src = "a";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.begin());
    }

    {
        std::string src = "b";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_logical_not() {
    const auto grammar = !p::terminal('a');

    {
        std::string src = "a";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }

    {
        std::string src = "b";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_sequence() {
    const auto grammar = p::terminal('a') >> 'b' >> 'c';

    {
        std::string src = "abc";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    {
        std::string src = "abd";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_choice() {
    const auto grammar = p::terminal('a') | 'b' | 'c';

    {
        std::string src = "a";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    {
        std::string src = "b";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    {
        std::string src = "c";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    {
        std::string src = "d";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_exclusion() {
    const auto grammar = p::terminal('a') - p::terminal('b');

    {
        std::string src = "a";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    {
        std::string src = "ba";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_any() {
    const auto grammar = p::any();

    {
        std::string src = "a";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    {
        std::string src = "b";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }
}


static void test_parse_end() {
    const auto grammar = p::terminal('a') >> p::end();

    {
        std::string src = "a";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    {
        std::string src = "ab";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_false() {
    const auto grammar = p::terminal('a') >> p::false_();

    {
        std::string src = "a";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }

    {
        std::string src = "b";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_true() {
    const auto grammar = p::terminal('a') | p::true_();

    {
        std::string src = "a";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    {
        std::string src = "b";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_function() {
    const auto grammar = p::function([](p::parse_context& pc) {
        return pc.is_valid_iterator() && *pc.get_iterator() == 'a';
    });

    {
        std::string src = "a";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.begin());
    }

    {
        std::string src = "b";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_newline() {
    using p = parser<text_iterator<>>;

    const auto grammar = p::terminal('a') >> p::newline('\n');

    {
        std::string src = "a\n";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
        assert(pc.get_iterator().get_line() == 2);
        assert(pc.get_iterator().get_column() == 1);
    }

    {
        std::string src = "a";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator().get_line() == 1);
        assert(pc.get_iterator().get_column() == 1);
    }
}


static void test_parse_match() {
    const auto a = p::terminal('a')->*1;
    const auto b = p::terminal('b')->*2;
    const auto ab = (a >> b)->*12;
    const auto c = p::terminal('c')->*3;
    const auto d = p::terminal('d')->*4;
    const auto grammar = *(ab | c | d);

    {
        std::string src = "abcd";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
        assert(pc.get_matches().size() == 3);

        assert(pc.get_matches()[0].get_id() == 12);
        assert(pc.get_matches()[0].begin() == src.begin());
        assert(pc.get_matches()[0].end() == std::next(src.begin(), 2));

        assert(pc.get_matches()[0].get_children().size() == 2);
        assert(pc.get_matches()[0].get_children()[0].get_id() == 1);
        assert(pc.get_matches()[0].get_children()[0].begin() == src.begin());
        assert(pc.get_matches()[0].get_children()[0].end() == std::next(src.begin(), 1));
        assert(pc.get_matches()[0].get_children()[1].get_id() == 2);
        assert(pc.get_matches()[0].get_children()[1].begin() == std::next(src.begin(), 1));
        assert(pc.get_matches()[0].get_children()[1].end() == std::next(src.begin(), 2));

        assert(pc.get_matches()[1].get_id() == 3);
        assert(pc.get_matches()[1].begin() == std::next(src.begin(), 2));
        assert(pc.get_matches()[1].end() == std::next(src.begin(), 3));

        assert(pc.get_matches()[2].get_id() == 4);
        assert(pc.get_matches()[2].begin() == std::next(src.begin(), 3));
        assert(pc.get_matches()[2].end() == std::next(src.begin(), 4));
    }
}


static void test_parse_error() {
    {
        const auto grammar = (p::terminal('a') >> p::terminal(';')) | p::error(1, p::skip_before(';'));

        {
            std::string src = "a;";
            p::parse_context pc(src);
            const bool ok = grammar.parse(pc);
            assert(ok);
            assert(pc.get_iterator() == src.end());
            assert(pc.get_errors().size() == 0);
        }

        {
            std::string src = "b;";
            p::parse_context pc(src);
            const bool ok = grammar.parse(pc);
            assert(ok);
            assert(pc.get_iterator() == std::prev(src.end()));
            assert(pc.get_errors().size() == 1);
            assert(pc.get_errors()[0].get_id() == 1);
            assert(pc.get_errors()[0].begin() == src.begin());
            assert(pc.get_errors()[0].end() == std::next(src.begin(), 1));
        }
    }

    {
        const auto grammar = (p::terminal('a') >> p::terminal(';')) | p::error(1, p::skip_after(';'));

        {
            std::string src = "a;";
            p::parse_context pc(src);
            const bool ok = grammar.parse(pc);
            assert(ok);
            assert(pc.get_iterator() == src.end());
            assert(pc.get_errors().size() == 0);
        }

        {
            std::string src = "b;";
            p::parse_context pc(src);
            const bool ok = grammar.parse(pc);
            assert(ok);
            assert(pc.get_iterator() == src.end());
            assert(pc.get_errors().size() == 1);
            assert(pc.get_errors()[0].get_id() == 1);
            assert(pc.get_errors()[0].begin() == src.begin());
            assert(pc.get_errors()[0].end() == src.end());
        }
    }
}


static void test_parse_case_insensitive() {
    using p = parser<std::string::const_iterator, int, int, case_insensitive_symbol_comparator>;

    const auto grammar = p::terminal('a');

    {
        std::string src = "a";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    {
        std::string src = "A";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    {
        std::string src = "b";
        p::parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }
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
    test_parse_exclusion();
    test_parse_any();
    test_parse_end();
    test_parse_false();
    test_parse_true();
    test_parse_function();
    test_parse_newline();
    test_parse_match();
    test_parse_error();
    test_parse_case_insensitive();
}
