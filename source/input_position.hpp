#ifndef PARSERLIB_INPUT_POSITION_HPP
#define PARSERLIB_INPUT_POSITION_HPP


#include "input_iterator.hpp"


namespace parserlib {


/** Input position.
    It contains an iterator and a source code position (line, column).
 */
class input_position {
public:
    ///empty constructor.
    input_position() {}
    
    /** constructor from input.
        The position points to the start of the input.
        @param buf input buffer.
     */
    input_position(input_buffer &buf);
    
    /** returns the iterator.
        @return the iterator.
     */
    const input_iterator &it() const;
    
    /** returns the line.
        @return the line.
     */
    size_t line() const;
    
    /** returns the column.
        @return the column.
     */
    size_t col() const;
    
    /** checks if this and the given position point to the same position in the input buffer.
        @param pos the other position.
        @return true if this and the given position are equal, false otherwise.
     */
    bool operator == (const input_position &pos) const;
    
    /** checks if this and the given position point to a different position in the input buffer.
        @param pos the other position.
        @return true if this and the given position are different, false otherwise.
     */
    bool operator != (const input_position &pos) const;
    
    /** checks if this position is lower in the buffer than the given position.
        @param pos the other position.
        @return true if this is lower in the buffer, false otherwise.
     */
    bool operator < (const input_position &pos) const;
    
    /** checks if this position is lower in the buffer than or equal to the given position.
        @param pos the other position.
        @return true if this and is lower or equal in the buffer, false otherwise.
     */
    bool operator <= (const input_position &pos) const;
    
    /** checks if this position is higher in the buffer than the given position.
        @param pos the other position.
        @return true if this is higher in the buffer, false otherwise.
     */
    bool operator > (const input_position &pos) const;
    
    /** checks if this position is higher in the buffer than or equal to the given position.
        @param pos the other position.
        @return true if this is higher or equal in the buffer, false otherwise.
     */
    bool operator >= (const input_position &pos) const;
    
    /** increments the iterator and column.
     */
    void next_col();
    
    /** increments the line and set the column to 1.
     */
    void next_line();

private:
    //iterator to input.
    input_iterator m_it;
    
    //line.
    size_t m_line;
    
    //column.
    size_t m_col;    
};


} //namespace parserlib


#endif //PARSERLIB_INPUT_POSITION_HPP
