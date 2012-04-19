#include "parse_context.hpp"
#include "rule.hpp"


namespace parserlib {


/** the default constructor.
    @param buf input buffer.
    @param ws whitespace rule.
 */
parse_context::parse_context(input_buffer &buf, rule &ws) : 
    m_input_end(buf.end()), m_error_position(buf), m_whitespace(ws)
{
}

    
/** returns the input end.
    @return the input end.
 */
const input_iterator &parse_context::input_end() const {
    return m_input_end;
}


/** checks if the given position does not point to the end of the input buffer.
    @param pos position.
    @return true if the given position does not point to the end of the input buffer, false otherwise.
 */
bool parse_context::valid_position(const input_position &pos) const {
    return pos.it() != m_input_end;
}


/** returns the error position.
    @return the error position.
 */
const input_position &parse_context::error_position() const {
    return m_error_position;
}


/** sets the error position, but only if the new position is higher than the last error position.
    This allows the parser to report the most further error.
    @param pos the new error position.
 */
void parse_context::set_error_position(const input_position &pos) {
    if (pos > m_error_position) {
        m_error_position = pos;
    }
}


/** parse whitespace.
    @param parent parent parse tree node; children nodes add themselves to this node.
    @param pos parse position; parsing continues from this position, if successful. 
 */
void parse_context::parse_whitespace(parse_node &parent, input_position &pos) {
    m_whitespace.parse(*this, parent, pos, false);
}


} //namespace parserlib
