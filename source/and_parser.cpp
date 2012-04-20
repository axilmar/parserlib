#include "and_parser.hpp"
#include "parser_state.hpp"


namespace parserlib {


/** the constructor.
    @param p child parser.
 */
and_parser::and_parser(const parser_object_ptr &p) :
    unary_parser(p)
{
}    


/** If the child parser returns true, it returns true, otherwise it returns false.
    The parser state after this method is the same as the state before calling this method.
    @param context the current parse context.
    @param parent parent parse tree node; children nodes add themselves to this node.
    @param pos parse position; parsing continues from this position, if successful. 
    @param parse_ws if true, whitespace is parsed between terminals.
    @return true if parsing succeeded, false otherwise.
    @exception left_recursion_success thrown if left recursion is successfully parsed.    
 */
bool and_parser::parse(parse_context &context, const parse_node_ptr &parent, input_position &pos, bool parse_ws) const {
    parser_state state(parent, pos);
    bool success = parser()->parse(context, parent, pos, parse_ws);
    state.restore(parent, pos);
    return success;
}


} //namespace parserlib
