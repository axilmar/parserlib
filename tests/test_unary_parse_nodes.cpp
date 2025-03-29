#include <string>
#include "parserlib/parse_context.hpp"
#include "parserlib/terminal_parse_nodes.hpp"
#include "parserlib/unary_parse_nodes.hpp"


using namespace parserlib;


static void test_zero_or_more_parse_node() {
    {
        const auto grammar = *terminal('a');
        std::string source = "a";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = *terminal('a');
        std::string source = "aa";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = *terminal('a');
        std::string source = "aaa";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = *terminal('a');
        std::string source = "";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.parse_position() == source.begin());
    }

    {
        const auto grammar = *terminal('a');
        std::string source = "bbb";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.parse_position() == source.begin());
    }
}


static void test_one_or_more_parse_node() {
    {
        const auto grammar = +terminal('a');
        std::string source = "a";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = +terminal('a');
        std::string source = "aa";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = +terminal('a');
        std::string source = "aaa";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = +terminal('a');
        std::string source = "";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(!grammar.parse(pc));
        assert(pc.parse_position() == source.begin());
    }

    {
        const auto grammar = +terminal('a');
        std::string source = "bbb";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(!grammar.parse(pc));
        assert(pc.parse_position() == source.begin());
    }
}


static void test_optional_parse_node() {
    {
        const auto grammar = -terminal('a');
        std::string source = "a";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = -terminal('a');
        std::string source = "";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.parse_position() == source.begin());
    }

    {
        const auto grammar = -terminal('a');
        std::string source = "b";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.parse_position() == source.begin());
    }
}


static void test_logical_and_parse_node() {
    {
        const auto grammar = &terminal('a');
        std::string source = "a";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.parse_position() == source.begin());
    }

    {
        const auto grammar = &terminal('a');
        std::string source = "";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(!grammar.parse(pc));
        assert(pc.parse_position() == source.begin());
    }

    {
        const auto grammar = &terminal('a');
        std::string source = "b";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(!grammar.parse(pc));
        assert(pc.parse_position() == source.begin());
    }
}


static void test_logical_not_parse_node() {
    {
        const auto grammar = !terminal('a');
        std::string source = "a";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(!grammar.parse(pc));
        assert(pc.parse_position() == source.begin());
    }

    {
        const auto grammar = !terminal('a');
        std::string source = "";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.parse_position() == source.begin());
    }

    {
        const auto grammar = !terminal('a');
        std::string source = "b";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.parse_position() == source.begin());
    }
}


void test_unary_parse_nodes() {
    test_zero_or_more_parse_node();
    test_one_or_more_parse_node();
    test_optional_parse_node();
    test_logical_and_parse_node();
    test_logical_not_parse_node();
}
