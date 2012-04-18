#ifndef PARSERLIB_STRING_PARSER_HPP
#define PARSERLIB_STRING_PARSER_HPP


#include "parser_object.hpp"


namespace parserlib {


/** Parses a string.
 */
class string_parser : public parser_object {
public:
    /** constructor.
        @param ch character to parse.
     */
    string_parser(input_char ch);
    
    /** constructor.
        @param str null-terminated string of characters to parse.
     */
    string_parser(const char *str);
    
    /** constructor.
        @param str null-terminated string of characters to parse.
     */
    string_parser(const wchar_t *str);
    
    /** Checks if input at the current position matches the string.
        If so, the position is incremented to the end of the matched string.
        @param context the current parse context.
        @param parent parent parse tree node; children nodes add themselves to this node.
        @param pos parse position; parsing continues from this position, if successful. 
        @return true if parsing succeeded, false otherwise.
     */
    virtual bool parse(parse_context &context, parse_node &parent, input_position &pos) const;

private:
    //string of characters
    input_buffer m_string;
}; 


} //namespace parserlib


#endif //PARSERLIB_STRING_PARSER_HPP
