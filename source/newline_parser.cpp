#include "newline_parser.hpp"
#include "parser_state.hpp"


namespace parserlib {


/** the constructor.
    @param p child parser.
 */
newline_parser::newline_parser(const parser_object_ptr &p) :
    unary_parser(p)
{
}    


/** If the child parser returns true, the line is incremented.
    @param context the current parse context.
    @param parent parent parse tree node; children nodes add themselves to this node.
    @param pos parse position; parsing continues from this position, if successful. 
    @param parse_ws if true, whitespace is parsed between terminals.
    @return true if parsing succeeded, false otherwise.
    @exception left_recursion_success thrown if left recursion is successfully parsed.    
 */
bool newline_parser::parse(parse_context &context, parse_node &parent, input_position &pos, bool parse_ws) const {
    if (parser()->parse(context, parent, pos, parse_ws)) {
        pos.next_line();
        return true;
    }
    return false;
}


} //namespace parserlib
