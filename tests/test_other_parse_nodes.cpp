#include <cassert>
#include <string>
#include "parserlib.hpp"


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
        const auto grammar = terminal(false);
        std::string source = "a";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(!grammar.parse(pc));
        assert(pc.parse_position() == source.begin());
    }

    {
        const auto grammar = terminal(true);
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
        const auto grammar = end();
        std::string source = "";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = end();
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


static void test_callback_parse_node() {
    struct test_extension {
        std::size_t count{ 0 };
    };

    const auto test_callback = [](auto& pc, const auto& result, const auto& start, const auto& end) {
        ++pc.extension().count;
        return result;
    };

    const auto grammar = terminal('a')[test_callback] >> terminal('b')[test_callback] >> terminal('c')[test_callback];

    std::string source = "abc";

    parse_context<std::string, int, int, case_sensitive_comparator, test_extension> pc(source, test_extension());

    assert(grammar.parse(pc));
    assert(pc.extension().count == 3);
}


void test_other_parse_nodes() {
    test_bool_parse_node();
    test_end_parse_node();
    test_function_parse_node();
    test_callback_parse_node();
}
