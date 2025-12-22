#include "parserlib.hpp"
using namespace parserlib;


void run_tests() {
	auto s = terminal('a') >> 'b' >> 'c' >> any() >> true >> debug(terminal('a')) >> end() >> newline('\n');
	auto c = terminal('a') | 'b' | 'c' | error(2, skip_after('a'));
	make_parse_node('a');
	terminal("a");
	make_parse_node("a");
	set("a");
	!range('a', 'z');
	terminal('a')->*1;
}
