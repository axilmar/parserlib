#ifndef PARSERLIB_RANGE_PARSER_HPP
#define PARSERLIB_RANGE_PARSER_HPP


#include "parser_object.hpp"


namespace parserlib {


/** Parses a character against a set of characters.
 */
class range_parser : public parser_object {
public:
    /** constructor.
        @param min min character of range.
        @param max max character of range, inclusive.
     */
    range_parser(input_char min, input_char max);
    
    /** Checks if the character at the current position belongs to the range of characters.
        If so, the column is incremented.
        @param context the current parse context.
        @param parent parent parse tree node; children nodes add themselves to this node.
        @param pos parse position; parsing continues from this position, if successful. 
        @return true if parsing succeeded, false otherwise.
     */
    virtual bool parse(parse_context &context, parse_node &parent, input_position &pos) const;

private:
    //range
    input_char m_min, m_max;

    //checks if the given character is within the range
    bool has_char(input_char ch) const;
}; 


} //namespace parserlib


#endif //PARSERLIB_RANGE_PARSER_HPP
