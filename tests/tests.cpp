#include <istream>
#include <sstream>
#include "parserlib.hpp"
using namespace parserlib;


void run_tests() {
	std::stringstream src;
	parse_context<std::istream_iterator<char>> p1{ std::istream_iterator<char>(src), std::istream_iterator<char>() };
	//p1.increment_text_position_line();
}
