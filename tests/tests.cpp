#include "parserlib/parse_context.hpp"


using namespace parserlib;


void run_tests() {
    std::string str = "abc";
    parse_context<> pc(str);
}
