#include <cassert>
#include <iostream>
#include "parserlib.hpp"
#include "calculator.hpp"


using namespace parserlib;
using namespace tests;


void test_ast_node() {
    calculator calc;
    std::string source = "2.0+1.0-3.0*4.0/5.0";
    parse_context<> context(source);
    assert(calc.grammar.parse(context));
    assert(context.is_end_parse_position());
    assert(context.matches().size() == 1);
    assert(calc.evaluate(context.matches()[0]) == 2.0 + 1.0 - 3.0 * 4.0 / 5.0);
    ast_node_ptr_type<parse_definitions<>> ast_tree = create_ast_node(context.matches()[0]);
    std::stringstream stream;
    ast_tree->print(stream, &calculator::get_id_name);
    //std::cout << stream.str();
}
