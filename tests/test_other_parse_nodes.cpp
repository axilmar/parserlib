#include "parserlib/parse_context.hpp"
#include "parserlib/terminal_parse_nodes.hpp"
#include "parserlib/other_parse_nodes.hpp"


using namespace parserlib;


static void test_bool_parse_node() {
    {
        const auto grammar = get_parse_node_wrapper(true);
        std::string source = "a";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.parse_position() == source.begin());
    }

    {
        const auto grammar = false_;
        std::string source = "a";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(!grammar.parse(pc));
        assert(pc.parse_position() == source.begin());
    }

    {
        const auto grammar = true_;
        std::string source = "a";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.parse_position() == source.begin());
    }

    {
        const auto grammar = terminal(false);
        std::string source = "a";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(!grammar.parse(pc));
        assert(pc.parse_position() == source.begin());
    }
}


static void test_end_parse_node() {
    {
        const auto grammar = end;
        std::string source = "";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = end;
        std::string source = "a";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(!grammar.parse(pc));
        assert(pc.parse_position() == source.begin());
    }
}


void test_other_parse_nodes() {
    test_bool_parse_node();
    test_end_parse_node();
}
