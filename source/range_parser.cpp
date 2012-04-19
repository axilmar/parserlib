#include "range_parser.hpp"


namespace parserlib {


/** constructor.
    @param min min character of range.
    @param max max character of range, inclusive.
 */
range_parser::range_parser(input_char min, input_char max) :
    m_min(min), m_max(max)
{
    assert(min <= max);
}    


/** Checks if the character at the current position belongs to the range of characters.
    If so, the column is incremented.
    @param context the current parse context.
    @param parent parent parse tree node; children nodes add themselves to this node.
    @param pos parse position; parsing continues from this position, if successful. 
    @param parse_ws if true, whitespace is parsed between terminals.
    @return true if parsing succeeded, false otherwise.
 */
bool range_parser::parse(parse_context &context, parse_node &parent, input_position &pos, bool parse_ws) const {
    if (context.valid_position(pos) && has_char(*pos.it())) {
        pos.next_col();
        return true;
    }
    context.set_error_position(pos);
    return false;
}


//checks if the given character is within the range
bool range_parser::has_char(input_char ch) const {
    return ch >= m_min && ch <= m_max;
}


} //namespace parserlib
