#ifndef PARSERLIB_EOF_PARSER_HPP
#define PARSERLIB_EOF_PARSER_HPP


#include "parser_object.hpp"


namespace parserlib {


/** a parser that tests if the whole input has been parsed.
 */
class eof_parser : public parser_object {
public:
    /** True if the end of input has been reached, false otherwise.
        The parser state after this method is the same as the state before calling this method.
        @param context the current parse context.
        @param parent parent parse tree node; children nodes add themselves to this node.
        @param pos parse position; parsing continues from this position, if successful. 
        @param parse_ws if true, whitespace is parsed between terminals.
        @return true if parsing succeeded, false otherwise.
        @exception left_recursion_success thrown if left recursion is successfully parsed.
     */
    virtual bool parse(parse_context &context, parse_node &parent, input_position &pos, bool parse_ws) const;
};


} //namespace parserlib


#endif //PARSERLIB_EOF_PARSER_HPP
