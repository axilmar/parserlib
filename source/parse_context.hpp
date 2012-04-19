#ifndef PARSERLIB_PARSE_CONTEXT_HPP
#define PARSERLIB_PARSE_CONTEXT_HPP


#include "parse_node.hpp"


namespace parserlib {


/** parse context.
 */
class parse_context {
public:
    /** the default constructor.
        @param buf input buffer.
        @param ws whitespace rule.
     */
    parse_context(input_buffer &buf, rule &ws);
    
    /** returns the input end.
        @return the input end.
     */
    const input_iterator &input_end() const;
    
    /** checks if the given position does not point to the end of the input buffer.
        @param pos position.
        @return true if the given position does not point to the end of the input buffer, false otherwise.
     */
    bool valid_position(const input_position &pos) const;
    
    /** returns the error position.
        @return the error position.
     */
    const input_position &error_position() const;
    
    /** sets the error position, but only if the new position is higher than the last error position.
        This allows the parser to report the most further error.
        @param pos the new error position.
     */
    void set_error_position(const input_position &pos);
    
    /** parse whitespace.
        @param parent parent parse tree node; children nodes add themselves to this node.
        @param pos parse position; parsing continues from this position, if successful. 
     */
    void parse_whitespace(parse_node &parent, input_position &pos);

private:
    //iterator to end of input buffer.
    input_iterator m_input_end;
    
    //the error position
    input_position m_error_position;
    
    //the whitespace rule
    rule &m_whitespace;
}; 


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_HPP
