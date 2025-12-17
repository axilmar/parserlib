#include <iostream>
#include <functional>
#include "parserlib.hpp"


using namespace parserlib;


#define ASSERT(FILE, LINE, COND) {\
    if ((COND) == false) {\
        std::cout << "Assertion failed: file " << FILE << ", line " << LINE << ": " << #COND << std::endl;\
    }\
}


#define DO_TEST(...) do_test(__FILE__, __LINE__, __VA_ARGS__)


struct test_match {
    size_t begin;
    size_t end;
    const char* text;
    int id;
};


struct test_error {
    size_t begin;
    size_t end;
    int id;
};


using text_parse_context = container_parse_context<std::string, int, int, text_source_position>;


struct any_value {
    template <class T>
    operator T() const {
        return T();
    }
};


#define _ any_value()


template <class Grammar>
static void do_test(
    const char* file,
    const int line,
    const Grammar& grammar, 
    const std::string& input, 
    bool test_result, 
    const std::vector<test_match>& test_matches = {}, 
    const std::vector<test_error>& test_errors = {},
    const std::function<bool(text_parse_context&)>& other_test = {})
{
    std::string source = input;
    text_parse_context context(source);
    const bool result = grammar.parse(context);
    
    //test the parse result
    ASSERT(file, line, result == test_result);

    //test the matches
    ASSERT(file, line, context.get_matches().size() == test_matches.size());
    for (size_t index = 0; index < test_matches.size(); ++index) {
        const auto& context_match = context.get_matches()[index];
        const struct test_match& test_match = test_matches[index];
        ASSERT(file, line, context_match.get_begin_parse_position().get_iterator() == std::next(source.begin(), test_match.begin));
        ASSERT(file, line, context_match.get_end_parse_position().get_iterator() == std::next(source.begin(), test_match.end));
        ASSERT(file, line, context_match.get_source<std::string>() == test_match.text);
        ASSERT(file, line, static_cast<int>(context_match.get_id()) == test_match.id);
    }

    //test the errors
    ASSERT(file, line, context.get_errors().size() == test_errors.size());
    for (size_t index = 0; index < test_errors.size(); ++index) {
        const auto& context_error = context.get_errors()[index];
        const struct test_error& test_error = test_errors[index];
        ASSERT(file, line, context_error.get_begin_parse_position().get_iterator() == std::next(source.begin(), test_error.begin));
        ASSERT(file, line, context_error.get_end_parse_position().get_iterator() == std::next(source.begin(), test_error.end));
        ASSERT(file, line, context_error.get_id() == test_error.id);
    }

    //other test
    if (other_test) {
        ASSERT(file, line, other_test(context));
    }
}


static void test_parse_symbol() {
    const auto grammar = terminal('a');
    DO_TEST(grammar, "a", true);
    DO_TEST(grammar, "b", false);
}


static void test_parse_string() {
    const auto grammar = terminal("abc");
    DO_TEST(grammar, "abc", true);
    DO_TEST(grammar, "abd", false);
}


static void test_parse_set() {
    const auto grammar = set("abc");
    DO_TEST(grammar, "a", true);
    DO_TEST(grammar, "b", true);
    DO_TEST(grammar, "c", true);
    DO_TEST(grammar, "A", false);
    DO_TEST(grammar, "d", false);
}


static void test_parse_range() {
    const auto grammar = range('0', '9');
    DO_TEST(grammar, "0", true);
    DO_TEST(grammar, "5", true);
    DO_TEST(grammar, "9", true);
    DO_TEST(grammar, "a", false);
    DO_TEST(grammar, "b", false);
}


static void test_parse_any() {
    const auto grammar = any();
    DO_TEST(grammar, "a", true);
    DO_TEST(grammar, "b", true);
    DO_TEST(grammar, "", false);
}


static void test_parse_end() {
    const auto grammar = end();
    DO_TEST(grammar, "", true);
    DO_TEST(grammar, "b", false);
}


static void test_parse_bool() {
    DO_TEST(terminal('a') >> true, "a", true);
    DO_TEST(terminal('a') >> false, "a", false);
}


static void test_parse_newline() {
    const auto grammar = newline('\n');
    DO_TEST(grammar, "\n", true, _, _, [](const auto& pc) { return pc.get_parse_position().get_source_position().get_line() == 2; });
    DO_TEST(grammar, "a", false, _, _, [](const auto& pc) { return pc.get_parse_position().get_source_position().get_line() == 1; });
}


static void test_parse_error() {
    {
        const auto grammar = terminal('a') >> ';' | error(1, skip_before(';'));
        DO_TEST(grammar, "a;", true);
        DO_TEST(grammar, "b;", true, _, { {0, 1, 1} }, [](const auto& pc) { return pc.get_parse_position().get_index() == 1; });
    }
    {
        const auto grammar = terminal('a') >> ';' | error(1, skip_after(';'));
        DO_TEST(grammar, "a;", true);
        DO_TEST(grammar, "b;", true, _, { {0, 2, 1} }, [](const auto& pc) { return pc.get_parse_position().get_index() == 2; });
    }
}


static void test_parse_loop_0() {
    const auto grammar = *terminal('a');
    DO_TEST(grammar, "a", true, _, _, [](const auto& pc) { return pc.is_end_parse_position(); });
    DO_TEST(grammar, "aa", true, _, _, [](const auto& pc) { return pc.is_end_parse_position(); });
    DO_TEST(grammar, "aaa", true, _, _, [](const auto& pc) { return pc.is_end_parse_position(); });
    DO_TEST(grammar, "", true, _, _, [](const auto& pc) { return pc.is_end_parse_position(); });
    DO_TEST(grammar, "b", true, _, _, [](const auto& pc) { return pc.get_parse_position().get_index() == 0; });
}


static void test_parse_loop_1() {
    const auto grammar = +terminal('a');
    DO_TEST(grammar, "a", true, _, _, [](const auto& pc) { return pc.is_end_parse_position(); });
    DO_TEST(grammar, "aa", true, _, _, [](const auto& pc) { return pc.is_end_parse_position(); });
    DO_TEST(grammar, "aaa", true, _, _, [](const auto& pc) { return pc.is_end_parse_position(); });
    DO_TEST(grammar, "ab", true, _, _, [](const auto& pc) { return pc.get_parse_position().get_index() == 1; });
    DO_TEST(grammar, "aab", true, _, _, [](const auto& pc) { return pc.get_parse_position().get_index() == 2; });
    DO_TEST(grammar, "", false, _, _, [](const auto& pc) { return pc.get_parse_position().get_index() == 0; });
    DO_TEST(grammar, "b", false, _, _, [](const auto& pc) { return pc.get_parse_position().get_index() == 0; });
}


static void test_parse_optional() {
    const auto grammar = -terminal('a');
    DO_TEST(grammar, "a", true, _, _, [](const auto& pc) { return pc.is_end_parse_position(); });
    DO_TEST(grammar, "b", true, _, _, [](const auto& pc) { return pc.get_parse_position().get_index() == 0; });
}


static void test_parse_logical_and() {
    const auto grammar = &terminal('a');
    DO_TEST(grammar, "a", true, _, _, [](const auto& pc) { return pc.get_parse_position().get_index() == 0; });
    DO_TEST(grammar, "b", false, _, _, [](const auto& pc) { return pc.get_parse_position().get_index() == 0; });
}


static void test_parse_logical_not() {
    const auto grammar = !terminal('a');
    DO_TEST(grammar, "a", false, _, _, [](const auto& pc) { return pc.get_parse_position().get_index() == 0; });
    DO_TEST(grammar, "b", true, _, _, [](const auto& pc) { return pc.get_parse_position().get_index() == 0; });
}


static void test_parse_sequence() {
    const auto grammar = terminal('a') >> 'b';
    DO_TEST(grammar, "ab", true, _, _, [](const auto& pc) { return pc.get_parse_position().get_index() == 2; });
    DO_TEST(grammar, "ac", false, _, _, [](const auto& pc) { return pc.get_parse_position().get_index() == 0; });
}


static void test_parse_choice() {
    const auto grammar = terminal('a') | 'b';
    DO_TEST(grammar, "a", true, _, _, [](const auto& pc) { return pc.get_parse_position().get_index() == 1; });
    DO_TEST(grammar, "b", true, _, _, [](const auto& pc) { return pc.get_parse_position().get_index() == 1; });
    DO_TEST(grammar, "c", false, _, _, [](const auto& pc) { return pc.get_parse_position().get_index() == 0; });
}


static void test_parse_match() {
    const auto grammar = +(terminal('a')->*1 | terminal('b')->*2);
    DO_TEST(grammar, "a", true, { {0, 1, "a", 1} });
    DO_TEST(grammar, "b", true, { {0, 1, "b", 2} });
    DO_TEST(grammar, "ab", true, { {0, 1, "a", 1}, {1, 2, "b", 2} });
    DO_TEST(grammar, "c", false);
}


static void test_parse_node_ptr() {
    const parse_node_ptr grammar = terminal('a');
    DO_TEST(grammar, "a", true);
    DO_TEST(grammar, "b", false);
}


static void test_parse_rule() {
    const rule grammar = terminal('a');
    DO_TEST(grammar, "a", true);
    DO_TEST(grammar, "b", false);
}


void run_tests() {
    test_parse_symbol();
    test_parse_string();
    test_parse_set();
    test_parse_range();
    test_parse_any();
    test_parse_end();
    test_parse_bool();
    test_parse_newline();
    test_parse_error();
    test_parse_loop_0();
    test_parse_loop_1();
    test_parse_optional();
    test_parse_logical_and();
    test_parse_logical_not();
    test_parse_sequence();
    test_parse_choice();
    test_parse_match();
    test_parse_node_ptr();
    test_parse_rule();
}
