#include "eof_parser.hpp"
#include "parser_state.hpp"
#include "parse_context.hpp"


namespace parserlib {


/** True if the end of input has been reached, false otherwise.
    The parser state after this method is the same as the state before calling this method.
    @param context the current parse context.
    @param parent parent parse tree node; children nodes add themselves to this node.
    @param pos parse position; parsing continues from this position, if successful. 
    @param parse_ws if true, whitespace is parsed between terminals.
    @return true if parsing succeeded, false otherwise.
    @exception left_recursion_success thrown if left recursion is successfully parsed.
 */
bool eof_parser::parse(parse_context &context, const parse_node_ptr &parent, input_position &pos, bool parse_ws) const {
    return context.end_position(pos);
}


} //namespace parserlib
