#include "set_parser.hpp"


namespace parserlib {


/** constructor.
    @param ch character to parse.
 */
set_parser::set_parser(input_char ch) {
    add_char(ch);
}


/** constructor.
    @param set null-terminated string of characters to parse.
 */
set_parser::set_parser(const char *set) {
    assert(set);
    for(; *set; ++set) {
        add_char(*set);
    }
}


/** constructor.
    @param set null-terminated string of characters to parse.
 */
set_parser::set_parser(const wchar_t *set) {
    assert(set);
    for(; *set; ++set) {
        add_char(*set);
    }
}


/** Checks if the character at the current position equals any of the characters in the set
    If so, the column is incremented.
    @param context the current parse context.
    @param parent parent parse tree node; children nodes add themselves to this node.
    @param pos parse position; parsing continues from this position, if successful. 
    @param parse_ws if true, whitespace is parsed between terminals.
    @return true if parsing succeeded, false otherwise.
 */
bool set_parser::parse(parse_context &context, parse_node &parent, input_position &pos, bool parse_ws) const {
    if (parse_ws) context.parse_whitespace(parent, pos);    
    if (context.valid_position(pos) && has_char(*pos.it())) {
        pos.next_col();
        return true;
    }
    context.set_error_position(pos);
    return false;
}


//adds a character to the set, resizing the boolean vector accordingly.
void set_parser::add_char(input_char ch) {
    if (ch >= m_set.size()) {
        m_set.resize(ch + 1);
    }
}


//checks if the given character is within the set
bool set_parser::has_char(input_char ch) const {
    return ch < m_set.size() && m_set[ch];
}


} //namespace parserlib
