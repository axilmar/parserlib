#include "term_parser.hpp"
#include "parser_state.hpp"


namespace parserlib {


/** the constructor.
    @param p child parser.
 */
term_parser::term_parser(const parser_object_ptr &p) :
    unary_parser(p)
{
}    


/** Turns off whitespace parsing during the call.
    @param context the current parse context.
    @param parent parent parse tree node; children nodes add themselves to this node.
    @param pos parse position; parsing continues from this position, if successful. 
    @param parse_ws if true, whitespace is parsed between terminals.
    @return true if parsing succeeded, false otherwise.
    @exception left_recursion_success thrown if left recursion is successfully parsed.    
 */
bool term_parser::parse(parse_context &context, parse_node &parent, input_position &pos, bool parse_ws) const {
    return parser()->parse(context, parent, pos, false);
}


} //namespace parserlib
