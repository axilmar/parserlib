#include "parserlib.hpp"
using namespace parserlib;


void run_tests() {
	terminal('a');
	make_parse_node('a');
	terminal("a");
	make_parse_node("a");
	set("a");
	range('a', 'z');
}
