#include <algorithm>
#include "parserlib/parse_context.hpp"
#include "parserlib/parse_nodes.hpp"


using namespace parserlib;


static void test_skip_error_handling() {
    {
        const auto grammar = terminal('a') >> ('b' | error(1, skip_to(';'))) >> ';';
        std::string source = "a;";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
        assert(pc.errors().size() == 1);
        assert(pc.errors()[0].id() == 1);
        assert(pc.errors()[0].begin() == std::next(source.begin(), 1));
        assert(pc.errors()[0].end() == std::next(source.begin(), 1));
    }

    {
        const auto grammar = terminal('a') >> ('b' | error(1, skip_after(';'))) >> ';';
        std::string source = "a;";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(!grammar.parse(pc));
        assert(pc.parse_position() == source.begin());
        assert(pc.errors().size() == 1);
        assert(pc.errors()[0].id() == 1);
        assert(pc.errors()[0].begin() == std::next(source.begin(), 1));
        assert(pc.errors()[0].end() == source.end());
    }

    {
        const auto grammar = terminal('a') >> ('b' | error(1, skip_current())) >> ';';
        std::string source = "ac;";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
        assert(pc.errors().size() == 1);
        assert(pc.errors()[0].id() == 1);
        assert(pc.errors()[0].begin() == std::next(source.begin(), 1));
        assert(pc.errors()[0].end() == std::next(source.begin(), 2));
    }
}


static void test_no_skip_error_handling() {
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
        std::string source = "ab;";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
        assert(pc.matches().size() == 2);
        assert(pc.matches()[0].id() == 1);
        assert(pc.matches()[0].begin() == source.begin());
        assert(pc.matches()[0].end() == std::next(source.begin(), 1));
        assert(pc.matches()[1].id() == 2);
        assert(pc.matches()[1].begin() == std::next(source.begin(), 1));
        assert(pc.matches()[1].end() == std::next(source.begin(), 2));
    }

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

    {
        const auto a = terminal('a')->*1;
        const auto b = terminal('b')->*2;
        const auto grammar = a >> error_match(b, 3) >> ';';
        std::string source = "ab;";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
        assert(pc.matches().size() == 2);
        assert(pc.matches()[0].id() == 1);
        assert(pc.matches()[0].begin() == source.begin());
        assert(pc.matches()[0].end() == std::next(source.begin(), 1));
        assert(pc.matches()[1].id() == 2);
        assert(pc.matches()[1].begin() == std::next(source.begin(), 1));
        assert(pc.matches()[1].end() == std::next(source.begin(), 2));
    }

    {
        const auto a = terminal('a')->*1;
        const auto b = terminal('b')->*2;
        const auto grammar = a >> error_match(b, 3) >> ';';
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
    test_skip_error_handling();
    test_no_skip_error_handling();
    test_error_match();
}
