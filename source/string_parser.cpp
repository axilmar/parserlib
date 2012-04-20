#include "string_parser.hpp"
#include "parse_context.hpp"


namespace parserlib {


/** constructor.
    @param ch character to parse.
 */
string_parser::string_parser(input_char ch) {
    m_string.push_back(ch);
}


/** constructor.
    @param str null-terminated string of characters to parse.
 */
string_parser::string_parser(const char *str) {
    for(; *str; ++str) {
        m_string.push_back(*str);
    }
}


/** constructor.
    @param str null-terminated string of characters to parse.
 */
string_parser::string_parser(const wchar_t *str) {
    for(; *str; ++str) {
        m_string.push_back(*str);
    }
}


/** Checks if input at the current position matches the string.
    If so, the position is incremented to the end of the matched string.
    @param context the current parse context.
    @param parent parent parse tree node; children nodes add themselves to this node.
    @param pos parse position; parsing continues from this position, if successful. 
    @param parse_ws if true, whitespace is parsed between terminals.
    @return true if parsing succeeded, false otherwise.
 */
bool string_parser::parse(parse_context &context, const parse_node_ptr &parent, input_position &pos, bool parse_ws) const {
    for(input_iterator it = m_string.begin(), end = m_string.end();;) {
    
        //if the whole string was compared, then we have a match
        if (it == end) {
            return true;
        }
        
        //if the context has a valid position, compare characters
        if (context.valid_position(pos)) {
        
            //if characters match, proceed with the next character
            if (*pos.it() == *it) {
                pos.next_col();
                ++it;
                continue;
            }
        }
        
        //no valid position or no character matched;
        //end with error
        break;
    }
    
    //end with error
    context.set_error_position(pos);
    return false;
}


} //namespace parserlib
