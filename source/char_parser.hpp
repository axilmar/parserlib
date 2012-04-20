#ifndef PARSERLIB_CHAR_PARSER_HPP
#define PARSERLIB_CHAR_PARSER_HPP


#include "parser_object.hpp"
#include "input_char.hpp"


namespace parserlib {


/** Parses a single character.
 */
class char_parser : public parser_object {
public:
    /** constructor.
        @param ch character to parse.
     */
    char_parser(input_char ch);
    
    /** Checks if the character at the current position equals the internal character.
        If so, the column is incremented.
        @param context the current parse context.
        @param parent parent parse tree node; children nodes add themselves to this node.
        @param pos parse position; parsing continues from this position, if successful. 
        @param parse_ws if true, whitespace is parsed between terminals.
        @return true if parsing succeeded, false otherwise.
     */
    virtual bool parse(parse_context &context, const parse_node_ptr &parent, input_position &pos, bool parse_ws) const;

private:
    //character to parse.
    input_char m_char;
}; 


} //namespace parserlib


#endif //PARSERLIB_CHAR_PARSER_HPP
