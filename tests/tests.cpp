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

    *terminal('a');
    +terminal('a');
    -terminal('a');
    &terminal('a');
    !terminal('a');

    auto s1 = terminal('a') >> 'b' >> 'c' >> "aaa" >> terminal('b');

    auto c1 = terminal('a') | 'b' | 'c' | "aaa" | terminal('b') >> true;

    auto c2 = terminal('a') | s1 | true | function([](interface::parse_context& pc) { return false; });

    error(1, skip_after('a'));

    rule r1 = 'a';
    rule r2 = r1;
}
