#include "extras/ebnf.hpp"


using namespace parserlib;


void test_ebnf() {
    std::string src = load_file("./../test.ebnf");

    {
        const auto result = ebnf::parse(src);
    }
}
