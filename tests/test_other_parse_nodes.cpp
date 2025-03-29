#include <cassert>
#include <string>
#include "parserlib/parse_context.hpp"
#include "parserlib/parse_nodes.hpp"


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


static bool is_1(parse_context<std::string, int, int, case_sensitive_comparator>& pc) {
    if (*pc.parse_position() == '1') {
        pc.increment_parse_position();
        return true;
    }
    return false;
}


static void test_function_parse_node() {
    const auto is_0 = [](auto& pc) {
        if (*pc.parse_position() == '0') {
            pc.increment_parse_position();
            return true;
        }
        return false;
    };

    {
        const auto grammar = function(is_0) >> function(&is_1);
        std::string source = "01";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }
}


void test_other_parse_nodes() {
    test_bool_parse_node();
    test_end_parse_node();
    test_function_parse_node();
}
