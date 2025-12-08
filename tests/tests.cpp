#include "parserlib.hpp"


using namespace parserlib;


void run_tests() {
    terminal('a');

    terminal("aaa");
    terminal(std::string("aaa"));
    std::string s;
    terminal(s);
    std::string_view sv(s);
    terminal(sv);
    terminal(std::string_view(s));

    set("aaa");
    set(std::string("123"));
    set({1, 2, 3});
    set(std::vector<int>({ 1, 2, 3 }));

    range('0', '9');
}
