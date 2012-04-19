#ifndef PARSERLIB_SET_PARSER_HPP
#define PARSERLIB_SET_PARSER_HPP


#include "parser_object.hpp"


namespace parserlib {


/** Parses a character against a set of characters.
 */
class set_parser : public parser_object {
public:
    /** constructor.
        @param ch character to parse.
     */
    set_parser(input_char ch);
    
    /** constructor.
        @param set null-terminated string of characters to parse.
     */
    set_parser(const char *set);
    
    /** constructor.
        @param set null-terminated string of characters to parse.
     */
    set_parser(const wchar_t *set);
    
    /** Checks if the character at the current position equals any of the characters in the set
        If so, the column is incremented.
        @param context the current parse context.
        @param parent parent parse tree node; children nodes add themselves to this node.
        @param pos parse position; parsing continues from this position, if successful. 
        @param parse_ws if true, whitespace is parsed between terminals.
        @return true if parsing succeeded, false otherwise.
     */
    virtual bool parse(parse_context &context, parse_node &parent, input_position &pos, bool parse_ws) const;

private:
    //the set of characters are represented as vectors of booleans, for performance reasons.
    std::vector<bool> m_set;
    
    //adds a character to the set, resizing the boolean vector accordingly.
    void add_char(input_char ch);
    
    //checks if the given character is within the set
    bool has_char(input_char ch) const;
}; 


} //namespace parserlib


#endif //PARSERLIB_SET_PARSER_HPP
