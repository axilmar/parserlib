#include "input_position.hpp"


namespace parserlib {


/** constructor from input.
    The position points to the start of the input.
    @param buf input buffer.
 */
input_position::input_position(input_buffer &buf) : m_it(buf.begin()), m_line(1), m_col(1) {
}

    
/** returns the iterator.
    @return the iterator.
 */
const input_iterator &input_position::it() const {
    return m_it;
}


/** returns the line.
    @return the line.
 */
size_t input_position::line() const {
    return m_line;
}


/** returns the column.
    @return the column.
 */
size_t input_position::col() const {
    return m_col;
}


/** checks if this and the given position point to the same position in the input buffer.
    @param pos the other position.
    @return true if this and the given position are equal, false otherwise.
 */
bool input_position::operator == (const input_position &pos) const {
    return m_it == pos.m_it;
}


/** checks if this and the given position point to a different position in the input buffer.
    @param pos the other position.
    @return true if this and the given position are different, false otherwise.
 */
bool input_position::operator != (const input_position &pos) const {
    return m_it != pos.m_it;
}


/** checks if this position is lower in the buffer than the given position.
    @param pos the other position.
    @return true if this is lower in the buffer, false otherwise.
 */
bool input_position::operator < (const input_position &pos) const {
    return m_it < pos.m_it;
}


/** checks if this position is lower in the buffer than or equal to the given position.
    @param pos the other position.
    @return true if this and is lower or equal in the buffer, false otherwise.
 */
bool input_position::operator <= (const input_position &pos) const {
    return m_it <= pos.m_it;
}


/** checks if this position is higher in the buffer than the given position.
    @param pos the other position.
    @return true if this is higher in the buffer, false otherwise.
 */
bool input_position::operator > (const input_position &pos) const {
    return m_it > pos.m_it;
}


/** checks if this position is higher in the buffer than or equal to the given position.
    @param pos the other position.
    @return true if this is higher or equal in the buffer, false otherwise.
 */
bool input_position::operator >= (const input_position &pos) const {
    return m_it >= pos.m_it;
}


/** increments the iterator and column.
 */
void input_position::next_col() {
    ++m_it;
    ++m_col;
}


/** increments the line and set the column to 1.
 */
void input_position::next_line() {
    ++m_line;
    m_col = 1;
}


} //namespace parserlib
