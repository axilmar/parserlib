#include <cassert>
#include "parserlib.hpp"


using namespace parserlib;


static void test_symbol() {
    symbol_ptr symbol1 = std::make_unique<typed_symbol<char>>('a');
    assert(symbol1->get_value() == 'a');

    std::string_view str1("abc");
    symbol_string_ptr symbol2 = std::make_unique<typed_symbol_string<char>>(str1);
    assert(symbol2->get_value() == std::vector<symbol_value_type>(str1.begin(), str1.end()));

    symbol_set_ptr symbol3 = std::make_unique<typed_symbol_set<char>>("9467182035");
    assert(symbol3->get_value() == std::vector<symbol_value_type>({ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' }));

    symbol_pair_ptr symbol4 = std::make_unique<typed_symbol_pair<char, char>>('0', '9');
    assert(symbol4->get_value() == std::make_pair(symbol_value_type('0'), symbol_value_type('9')));
}


static void test_parse_symbol() {
    const rule grammar = 'a';

    //true
    {
        std::string src = "a";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    //false
    {
        std::string src = "b";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_string() {
    const rule grammar = "abc";

    //true
    {
        std::string src = "abc";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    //false
    {
        std::string src = "abd";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_set() {
    const rule grammar = set("5270698143");

    //true
    {
        std::string src = "0";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    //true
    {
        std::string src = "5";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    //true
    {
        std::string src = "9";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    //false
    {
        std::string src = "a";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_range() {
    const rule grammar = range('0', '9');

    //true
    {
        std::string src = "0";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    //true
    {
        std::string src = "5";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    //true
    {
        std::string src = "9";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    //false
    {
        std::string src = "a";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_loop_0() {
    const rule grammar = *terminal('a');

    //true
    {
        std::string src = "";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    //true
    {
        std::string src = "a";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    //true
    {
        std::string src = "aa";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    //true, no parsing
    {
        std::string src = "b";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_loop_1() {
    const rule grammar = +terminal('a');

    //true
    {
        std::string src = "a";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    //true
    {
        std::string src = "aa";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    //false
    {
        std::string src = "b";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_optional() {
    const rule grammar = -terminal('a');

    //true
    {
        std::string src = "a";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    //false
    {
        std::string src = "b";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_logical_and() {
    const rule grammar = &terminal('a');

    //true
    {
        std::string src = "a";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.begin());
    }

    //false
    {
        std::string src = "b";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_logical_not() {
    const rule grammar = !terminal('a');

    //true
    {
        std::string src = "a";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }

    //false
    {
        std::string src = "b";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_sequence() {
    const rule grammar = terminal('a') >> 'b' >> 'c';

    //true
    {
        std::string src = "abc";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    //false
    {
        std::string src = "abd";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_choice() {
    const rule grammar = terminal('a') | 'b' | 'c';

    //true
    {
        std::string src = "a";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    //true
    {
        std::string src = "b";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    //true
    {
        std::string src = "c";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    //false
    {
        std::string src = "0";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_parse_match() {
    enum class MATCH_ID {
        A,
        B,
        C,
        AB,
        AC
    };

    const auto a = terminal('a')->*MATCH_ID::A;
    const auto b = terminal('b')->*MATCH_ID::B;
    const auto c = terminal('c')->*MATCH_ID::C;

    const auto ab = (a >> b)->*MATCH_ID::AB;
    const auto ac = (a >> c)->*MATCH_ID::AC;

    const auto grammar = *(ab | ac);

    std::string src = "abac";
    parse_context<std::string::const_iterator, MATCH_ID> pc(src);
    const bool ok = grammar.parse(pc);
    assert(ok);
    assert(pc.get_iterator() == src.end());
    assert(pc.get_matches().size() == 2);

    assert(pc.get_matches()[0].get_id() == MATCH_ID::AB);
    assert(pc.get_matches()[0].get_children().size() == 2);
    assert(pc.get_matches()[0].get_children()[0].get_id() == MATCH_ID::A);
    assert(pc.get_matches()[0].get_children()[1].get_id() == MATCH_ID::B);

    assert(pc.get_matches()[1].get_id() == MATCH_ID::AC);
    assert(pc.get_matches()[1].get_children().size() == 2);
    assert(pc.get_matches()[1].get_children()[0].get_id() == MATCH_ID::A);
    assert(pc.get_matches()[1].get_children()[1].get_id() == MATCH_ID::C);
}


static void test_parse_exclusion() {
    const rule grammar = any - 'a';

    //true
    {
        std::string src = "b";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    //false
    {
        std::string src = "a";
        parse_context pc(src);
        const bool ok = grammar.parse(pc);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }
}


static void test_rule() {
    //test rule where its expression is set before it is used
    {
        rule r = 'a';
        auto grammar = +r;

        //true
        {
            std::string src = "a";
            parse_context pc(src);
            const bool ok = grammar.parse(pc);
            assert(ok);
            assert(pc.get_iterator() == src.end());
        }

        //true
        {
            std::string src = "aa";
            parse_context pc(src);
            const bool ok = grammar.parse(pc);
            assert(ok);
            assert(pc.get_iterator() == src.end());
        }

        //false
        {
            std::string src = "b";
            parse_context pc(src);
            const bool ok = grammar.parse(pc);
            assert(!ok);
            assert(pc.get_iterator() == src.begin());
        }
    }

    //test rule where its expression is set after it is used
    {
        rule r;
        auto grammar = +r;
        r = 'a';

        //true
        {
            std::string src = "a";
            parse_context pc(src);
            const bool ok = grammar.parse(pc);
            assert(ok);
            assert(pc.get_iterator() == src.end());
        }

        //true
        {
            std::string src = "aa";
            parse_context pc(src);
            const bool ok = grammar.parse(pc);
            assert(ok);
            assert(pc.get_iterator() == src.end());
        }

        //false
        {
            std::string src = "b";
            parse_context pc(src);
            const bool ok = grammar.parse(pc);
            assert(!ok);
            assert(pc.get_iterator() == src.begin());
        }
    }

    //test rule recursive declaration at initialization
    {
        rule grammar
            = 'a' >> grammar
            | end
            ;

        //true
        {
            std::string src = "a";
            parse_context pc(src);
            const bool ok = grammar.parse(pc);
            assert(ok);
            assert(pc.get_iterator() == src.end());
        }

        //true
        {
            std::string src = "aa";
            parse_context pc(src);
            const bool ok = grammar.parse(pc);
            assert(ok);
            assert(pc.get_iterator() == src.end());
        }

        //false
        {
            std::string src = "b";
            parse_context pc(src);
            const bool ok = grammar.parse(pc);
            assert(!ok);
            assert(pc.get_iterator() == src.begin());
        }
    }

    //test rule recursive declaration after initialization
    {
        rule grammar;

        grammar
            = 'a' >> grammar
            | end
            ;

        //true
        {
            std::string src = "a";
            parse_context pc(src);
            const bool ok = grammar.parse(pc);
            assert(ok);
            assert(pc.get_iterator() == src.end());
        }

        //true
        {
            std::string src = "aa";
            parse_context pc(src);
            const bool ok = grammar.parse(pc);
            assert(ok);
            assert(pc.get_iterator() == src.end());
        }

        //false
        {
            std::string src = "b";
            parse_context pc(src);
            const bool ok = grammar.parse(pc);
            assert(!ok);
            assert(pc.get_iterator() == src.begin());
        }
    }
}


void run_tests() {
    test_symbol();
    test_parse_symbol();
    test_parse_string();
    test_parse_set();
    test_parse_range();
    test_parse_loop_0();
    test_parse_loop_1();
    test_parse_optional();
    test_parse_logical_and();
    test_parse_logical_not();
    test_parse_sequence();
    test_parse_choice();
    test_parse_match();
    test_parse_exclusion();
    test_rule();
}
