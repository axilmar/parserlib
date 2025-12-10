#include <cassert>
#include <algorithm>
#include "parserlib.hpp"


using namespace parserlib;


static void test_symbol() {
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
        assert(pc.get_parse_position().get_iterator() == source.begin());
    }
}


static void test_string() {
    const auto grammar = terminal("aaa");

    {
        std::string source = "aaa";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "aab";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.get_parse_position().get_iterator() == source.begin());
    }
}


static void test_set() {
    const auto grammar = set("abc");

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
        assert(pc.get_parse_position().get_iterator() == source.begin());
    }
}


static void test_range() {
    const auto grammar = range('0', '9');

    {
        std::string source = "0";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "5";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "9";
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
        assert(pc.get_parse_position().get_iterator() == source.begin());
    }
}


static void test_loop0() {
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
        assert(pc.get_parse_position().get_iterator() == source.begin());
    }

    {
        std::string source = "ab";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_parse_position().get_iterator() == std::next(source.begin(), 1));
    }

    {
        std::string source = "aab";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_parse_position().get_iterator() == std::next(source.begin(), 2));
    }
}


static void test_loop1() {
    const auto grammar = +terminal('a');

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
        std::string source = "";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.get_parse_position().get_iterator() == source.begin());
    }

    {
        std::string source = "b";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.get_parse_position().get_iterator() == source.begin());
    }

    {
        std::string source = "ab";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_parse_position().get_iterator() == std::next(source.begin(), 1));
    }

    {
        std::string source = "aab";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_parse_position().get_iterator() == std::next(source.begin(), 2));
    }
}


static void test_optional() {
    const auto grammar = -terminal('a');

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
        assert(pc.get_parse_position().get_iterator() == source.begin());
    }
}


static void test_logical_and() {
    const auto grammar = &terminal('a');

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_parse_position().get_iterator() == source.begin());
    }

    {
        std::string source = "b";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.get_parse_position().get_iterator() == source.begin());
    }
}


static void test_logical_not() {
    const auto grammar = !terminal('a');

    {
        std::string source = "b";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_parse_position().get_iterator() == source.begin());
    }

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.get_parse_position().get_iterator() == source.begin());
    }
}


static void test_sequence() {
    const auto grammar = terminal('a') >> 'b' >> 'c';

    {
        std::string source = "abc";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
    }

    {
        std::string source = "ab1";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.get_parse_position().get_iterator() == source.begin());
    }
}


static void test_choice() {
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
        std::string source = "1";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.get_parse_position().get_iterator() == source.begin());
    }
}


static void test_any() {
    const auto grammar = any();

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
        std::string source = "";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.get_parse_position().get_iterator() == source.begin());
    }
}


static void test_bool() {
    {
        const auto grammar = terminal('a') >> false;

        {
            std::string source = "a";
            parse_context<> pc(source);
            const bool result = grammar.parse(pc);
            assert(!result);
            assert(pc.get_parse_position().get_iterator() == source.begin());
        }

        {
            std::string source = "b";
            parse_context<> pc(source);
            const bool result = grammar.parse(pc);
            assert(!result);
            assert(pc.get_parse_position().get_iterator() == source.begin());
        }
    }

    {
        const auto grammar = terminal('a') >> true;

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
            assert(pc.get_parse_position().get_iterator() == source.begin());
        }
    }
}


static void test_end() {
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
        assert(pc.get_parse_position().get_iterator() == source.begin());
    }
}


static void test_error() {
    enum class ERROR_ID { SYNTAX_ERROR };

    const auto grammar = terminal('a') | error(ERROR_ID::SYNTAX_ERROR, any());

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
        assert(pc.get_errors().size() == 0);
    }

    {
        std::string source = "b";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_parse_position().get_iterator() == source.end());
        assert(pc.get_errors().size() == 1);
        assert(pc.get_errors()[0].get_begin_parse_position().get_iterator() == source.begin());
        assert(pc.get_errors()[0].get_end_parse_position().get_iterator() == std::next(source.begin(), 1));
    }
}


static void test_skip_after() {
    enum class ERROR_ID { SYNTAX_ERROR };

    const auto grammar = terminal('a') | error(ERROR_ID::SYNTAX_ERROR, skip_after(';'));

    {
        std::string source = "b;";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_parse_position().get_iterator() == source.end());
        assert(pc.get_errors().size() == 1);
        assert(pc.get_errors()[0].get_begin_parse_position().get_iterator() == source.begin());
        assert(pc.get_errors()[0].get_end_parse_position().get_iterator() == std::next(source.begin(), 2));
    }
}


static void test_skip_before() {
    enum class ERROR_ID { SYNTAX_ERROR };

    const auto grammar = terminal('a') | error(ERROR_ID::SYNTAX_ERROR, skip_before(';'));

    {
        std::string source = "b;";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.get_parse_position().get_iterator() == std::next(source.begin(), 1));
        assert(pc.get_errors().size() == 1);
        assert(pc.get_errors()[0].get_begin_parse_position().get_iterator() == source.begin());
        assert(pc.get_errors()[0].get_end_parse_position().get_iterator() == std::next(source.begin(), 1));
    }
}


static void test_function() {
    const auto grammar = function([](interface::parse_context& pc) {
        if (pc.is_valid_parse_position() && pc.compare_symbols(pc.get_current_symbol(), 'a') == 0) {
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
        assert(pc.get_parse_position().get_iterator() == source.begin());
    }
}


static void test_match() {
    enum class MATCH_ID { A };

    const auto grammar = terminal('a')->*MATCH_ID::A;

    {
        std::string source = "a";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
        assert(pc.get_matches().size() == 1);
        assert(pc.get_matches()[0].get_begin_parse_position().get_iterator() == source.begin());
        assert(pc.get_matches()[0].get_end_parse_position().get_iterator() == source.end());
    }

    {
        std::string source = "b";
        parse_context<> pc(source);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.get_matches().size() == 0);
    }
}


static void test_newline() {
    enum class MATCH_ID { A };

    const auto grammar = newline('\n') >> terminal('a')->*MATCH_ID::A;

    {
        std::string source = "\na";
        parse_context<std::string, int, int, file_position> pc(source);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.is_end_parse_position());
        assert(pc.get_matches().size() == 1);
        assert(pc.get_matches()[0].get_begin_parse_position().get_source_position().get_line() == 2);
        assert(pc.get_matches()[0].get_begin_parse_position().get_source_position().get_column() == 1);
    }
}


static void test_rule() {
    rule grammar = terminal('a');

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
        assert(pc.get_parse_position().get_iterator() == source.begin());
    }
}


void run_tests() {
    test_symbol();
    test_string();
    test_set();
    test_range();
    test_loop0();
    test_loop1();
    test_optional();
    test_logical_and();
    test_logical_not();
    test_sequence();
    test_choice();
    test_any();
    test_bool();
    test_end();
    test_error();
    test_skip_after();
    test_skip_before();
    test_function();
    test_match();
    test_newline();
    test_rule();
}
