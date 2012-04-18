#include "parse_context.hpp"


namespace parserlib {


/** the default constructor.
    @param buf input buffer.
 */
parse_context::parse_context(input_buffer &buf) : 
    m_input_end(buf.end()), m_error_position(buf)
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


} //namespace parserlib
