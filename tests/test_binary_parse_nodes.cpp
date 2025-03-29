#include <string>
#include "parserlib/parse_context.hpp"
#include "parserlib/terminal_parse_nodes.hpp"
#include "parserlib/binary_parse_nodes.hpp"


using namespace parserlib;


static void test_sequence_parse_node() {
    {
        const auto grammar = terminal('a') >> 'b';
        std::string source = "ab";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = 'a' >> terminal('b');
        std::string source = "ab";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = terminal('a') >> terminal('b');
        std::string source = "ab";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = (terminal('a') >> terminal('b')) >> 'c';
        std::string source = "abc";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = terminal('a') >> (terminal('b') >> 'c');
        std::string source = "abc";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = (terminal('a') >> 'b') >> (terminal('c') >> 'd');
        std::string source = "abcd";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = terminal('a') >> 'b';
        std::string source = "ac";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(!grammar.parse(pc));
        assert(pc.parse_position() == source.begin());
    }
}


static void test_choice_parse_node() {
    {
        const auto grammar = terminal('a') | 'b';
        std::string source = "a";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = 'a' | terminal('b');
        std::string source = "b";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = terminal('a') | terminal('b');
        std::string source = "a";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = (terminal('a') | terminal('b')) | 'c';
        std::string source = "b";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = terminal('a') | (terminal('b') | 'c');
        std::string source = "c";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = (terminal('a') | 'b') | (terminal('c') | 'd');
        std::string source = "d";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        const auto grammar = terminal('a') | 'b';
        std::string source = "f";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(!grammar.parse(pc));
        assert(pc.parse_position() == source.begin());
    }
}


static void test_match_parse_node() {
    {
        const auto grammar = terminal('a')->*1;
        std::string source = "a";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc));
        assert(pc.is_end_parse_position());
        assert(pc.matches().size() == 1);
        assert(pc.matches()[0].begin() == source.begin());
        assert(pc.matches()[0].end() == source.end());
    }

    {
        const auto grammar = terminal('a')->*1;
        std::string source = "b";
        parse_context<std::string, int, int, case_sensitive_comparator> pc(source);
        assert(grammar.parse(pc) == false);
        assert(pc.parse_position() == source.begin());
        assert(pc.matches().size() == 0);
    }
}


void test_binary_parse_nodes() {
    test_sequence_parse_node();
    test_choice_parse_node();
    test_match_parse_node();
}