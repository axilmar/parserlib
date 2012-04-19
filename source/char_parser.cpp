#include "char_parser.hpp"


namespace parserlib {


/** constructor.
    @param ch character to parse.
 */
char_parser::char_parser(input_char ch) : m_char(ch) {
}


/** Checks if the character at the current position equals the internal character.
    If so, the column is incremented.
    @param context the current parse context.
    @param parent parent parse tree node; children nodes add themselves to this node.
    @param pos parse position; parsing continues from this position, if successful. 
    @param parse_ws if true, whitespace is parsed between terminals.
    @return true if parsing succeeded, false otherwise.
 */
bool char_parser::parse(parse_context &context, parse_node &parent, input_position &pos, bool parse_ws) const {
    if (context.valid_position(pos) && *pos.it() == m_char) {
        pos.next_col();
        return true;
    }
    context.set_error_position(pos);
    return false;
}


} //namespace parserlib
