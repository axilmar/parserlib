#include "parserlib.hpp"


using namespace parserlib;


void run_tests() {
    parser<>::terminal('a');
    parser<>::terminal("abc");
    parser<>::set("abc");
    parser<>::range('0', '9');
}
