#include <cstdio>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include "parserlib.hpp"


using namespace parserlib;


#undef assert
#define assert(COND)\
    if (!(COND)) {\
        throw assertion_failed(__FILE__, __LINE__, #COND);\
    }\


#define DO_TEST(TEST)\
    try {\
        TEST();\
    }\
    catch (const std::exception& ex) {\
        std::cout << ex.what() << std::endl;\
    }\
    catch (...) {\
        std::cout << "exception happened\n";\
    }


class assertion_failed : public std::exception {
public:
    assertion_failed(const char* file, int line, const char* cond) : m_message(get_message(file, line, cond)) {
    }

    const char* what() const noexcept override {
        return m_message.c_str();
    }

private:
    std::string m_message;

    static std::string get_message(const char* file, int line, const char* cond) {
        std::stringstream stream;
        stream << "Assertion failed at file " << file << ", line " << line << ": " << cond;
        return stream.str();
    }
};


static void test_parse_symbol() {
    const auto grammar = terminal('a');

    {
        std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "b";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.parse_position().iterator() == input.begin());
    }
}


static void test_parse_string() {
    const auto grammar = terminal("abc");

    {
        std::string input = "abc";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.parse_position().iterator() == input.begin());
    }
}


static void test_parse_symbol_set() {
    const auto grammar = set("0123456789");

    {
        std::string input = "0";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "5";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "9";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(result);
        assert(pc.parse_position().iterator() == input.end());
    }

    {
        std::string input = "a";
        parse_context<> pc(input);
        const bool result = grammar.parse(pc);
        assert(!result);
        assert(pc.parse_position().iterator() == input.begin());
    }
}


void run_tests() {
    DO_TEST(test_parse_symbol);
    DO_TEST(test_parse_string);
    DO_TEST(test_parse_symbol_set);
}
