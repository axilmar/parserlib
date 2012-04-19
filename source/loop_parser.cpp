#include "loop_parser.hpp"
#include "parser_state.hpp"


namespace parserlib {


/** the constructor.
    @param p child parser.
 */
loop_parser::loop_parser(const parser_object_ptr &p) :
    unary_parser(p)
{
}    


/** parses the child parser zero or more times.
    @param context the current parse context.
    @param parent parent parse tree node; children nodes add themselves to this node.
    @param pos parse position; parsing continues from this position, if successful. 
    @param parse_ws if true, whitespace is parsed between terminals.
    @return always true.
    @exception left_recursion_success thrown if left recursion is successfully parsed.
 */
bool loop_parser::parse(parse_context &context, parse_node &parent, input_position &pos, bool parse_ws) const {
    for(;;) {
        parser_state state(parent, pos);
        if (parser()->parse(context, parent, pos, parse_ws)) continue;
        state.restore(parent, pos);
        break;
    }
    return true;
}


} //namespace parserlib
