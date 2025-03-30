#include <algorithm>
#include "parserlib/parse_context.hpp"
#include "parserlib/parse_nodes.hpp"


using namespace parserlib;


static void test_skip() {
    {
        const auto grammar = terminal('a') >> ('b' | skip_until(';')) >> ';';
        std::string source = "a;";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = terminal('a') >> ('b' | skip_until_after('@')) >> ';';
        std::string source = "a@;";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = terminal('a') >> ('b' | skip_while('@')) >> ';';
        std::string source = "a@@@@;";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = terminal('a') >> ('b' | skip_current()) >> ';';
        std::string source = "a@;";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }
}


static void test_error_handler() {
    {
        const auto grammar = terminal('a') >> ('b' | error(1)) >> ';';
        std::string source = "a;";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
        assert(pc.errors().size() == 1);
        assert(pc.errors()[0].id() == 1);
        assert(pc.errors()[0].begin() == std::next(source.begin(), 1));
        assert(pc.errors()[0].end() == std::next(source.begin(), 1));
    }
}


static void test_error_match() {
    {
        const auto a = terminal('a')->*1;
        const auto b = terminal('b')->*2;
        const auto grammar = a >> (b | error_match(3)) >> ';';
        std::string source = "a;";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
        assert(pc.matches().size() == 2);
        assert(pc.matches()[0].id() == 1);
        assert(pc.matches()[0].begin() == source.begin());
        assert(pc.matches()[0].end() == std::next(source.begin(), 1));
        assert(pc.matches()[1].id() == 3);
        assert(pc.matches()[1].begin() == std::next(source.begin(), 1));
        assert(pc.matches()[1].end() == std::next(source.begin(), 1));
    }
}


void test_error_handling() {
    test_skip();
    test_error_handler();
    test_error_match();
}
