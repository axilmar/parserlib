#include "not_parser.hpp"
#include "parser_state.hpp"


namespace parserlib {


/** the constructor.
    @param p child parser.
 */
not_parser::not_parser(const parser_object_ptr &p) :
    unary_parser(p)
{
}    


/** If the child parser returns true, it returns false, otherwise it returns true.
    The parser state after this method is the same as the state before calling this method.
    @param context the current parse context.
    @param parent parent parse tree node; children nodes add themselves to this node.
    @param pos parse position; parsing continues from this position, if successful. 
    @param parse_ws if true, whitespace is parsed between terminals.
    @return true if parsing succeeded, false otherwise.
    @exception left_recursion_success thrown if left recursion is successfully parsed.
 */
bool not_parser::parse(parse_context &context, parse_node &parent, input_position &pos, bool parse_ws) const {
    parser_state state(parent, pos);
    bool success = !parser()->parse(context, parent, pos, parse_ws);
    state.restore(parent, pos);
    return success;
}


} //namespace parserlib
