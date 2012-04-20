#include "optional_parser.hpp"
#include "parser_state.hpp"


namespace parserlib {


/** the constructor.
    @param p child parser.
 */
optional_parser::optional_parser(const parser_object_ptr &p) :
    unary_parser(p)
{
}    


/** If the child parser fails to parse, the parser state is restored to the one before calling this method.
    @param context the current parse context.
    @param parent parent parse tree node; children nodes add themselves to this node.
    @param pos parse position; parsing continues from this position, if successful. 
    @param parse_ws if true, whitespace is parsed between terminals.
    @return always true.
    @exception left_recursion_success thrown if left recursion is successfully parsed.
 */
bool optional_parser::parse(parse_context &context, const parse_node_ptr &parent, input_position &pos, bool parse_ws) const {
    parser_state state(parent, pos);
    if (parser()->parse(context, parent, pos, parse_ws)) return true;
    state.restore(parent, pos);
    return true;
}


} //namespace parserlib
