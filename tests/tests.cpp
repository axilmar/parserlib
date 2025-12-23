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
    const auto grammar = *(newline('\n') | terminal('a'));

    {
        std::string source = "a\na";
        parse_context<parse_iterator<std::string::const_iterator, file_text_position>> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator().get_text_position().get_line() == 2);
        assert(pc.get_iterator().get_text_position().get_column() == 2);
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
    using PARSE_CONTEXT = parse_context<>;

    parse_node_ptr<PARSE_CONTEXT> grammar = terminal('a');

    {
        std::string source = "a";
        PARSE_CONTEXT pc(source);
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
    using PARSE_CONTEXT = parse_context<>;

    rule<PARSE_CONTEXT> grammar = terminal('a');

    {
        std::string source = "a";
        PARSE_CONTEXT pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == source.end());
    }

    {
        std::string source = "b";
        PARSE_CONTEXT pc(source);
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
    using PARSE_CONTEXT = parse_context<>;

    const rule<PARSE_CONTEXT> grammar = -(terminal('a') >> grammar);

    {
        std::string source = "";
        PARSE_CONTEXT pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "a";
        PARSE_CONTEXT pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "aa";
        PARSE_CONTEXT pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "aaa";
        PARSE_CONTEXT pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "b";
        PARSE_CONTEXT pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == source.begin());
    }

    {
        std::string source = "ab";
        PARSE_CONTEXT pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == std::next(source.begin(), 1));
    }

    {
        std::string source = "aab";
        PARSE_CONTEXT pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_iterator() == std::next(source.begin(), 2));
    }
}


static void test_parse_left_recursion() {
    //TODO
}


void run_tests() {
    /*
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
    */
    test_parse_left_recursion();
}
