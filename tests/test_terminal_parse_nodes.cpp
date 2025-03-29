#include <string>
#include "parserlib/parse_context.hpp"
#include "parserlib/terminal_parse_nodes.hpp"


using namespace parserlib;


static void test_terminal_parse_node() {
    {
        const auto grammar = terminal('a');
        std::string source = "a";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = terminal('a');
        std::string source = "b";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc) == false);
        assert(pc.parse_position() == source.begin());
    }
}


static void test_terminal_sequence_parse_node() {
    {
        const auto grammar = terminal("abc");
        std::string source = "abc";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = terminal("abc");
        std::string source = "abb";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc) == false);
        assert(pc.parse_position() == source.begin());
    }
}


static void test_terminal_choice_parse_node() {
    {
        const auto grammar = set("abc");
        std::string source = "a";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = set("abc");
        std::string source = "b";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = set("abc");
        std::string source = "c";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = set("abc");
        std::string source = "d";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc) == false);
        assert(pc.parse_position() == source.begin());
    }
}


static void test_terminal_range_parse_node() {
    {
        const auto grammar = range('a', 'z');
        std::string source = "a";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = range('a', 'z');
        std::string source = "1";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc) == false);
        assert(pc.parse_position() == source.begin());
    }
}


void test_terminal_parse_nodes() {
    test_terminal_parse_node();
    test_terminal_sequence_parse_node();
    test_terminal_choice_parse_node();
    test_terminal_range_parse_node();
}
