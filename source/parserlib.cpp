#include <iostream>
#include "parserlib.hpp"
#include "parse_context.hpp"


namespace parserlib {


/** parses an input.
    @param input input to parse.
    @param grammar root rule of the grammar.
    @param ws whitespace rule.
    @return true on success, false on failure.
 */
bool parse(input_buffer &input, rule &grammar, rule &ws) {
    //the parse context
    parse_context context(input, ws);
    
    //the initial position
    input_position pos(input);
    
    //parse
    parse_node_ptr parse_tree;
    bool success = grammar.parse(context, pos, true, parse_tree);
    
    //TODO remove
    if (parse_tree) parse_tree->print(std::cout);
    
    //return success
    return success;
}


} //namespace parserlib
