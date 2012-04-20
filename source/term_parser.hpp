#ifndef PARSERLIB_TERM_PARSER_HPP
#define PARSERLIB_TERM_PARSER_HPP


#include "unary_parser.hpp"


namespace parserlib {


/** a parser that turns off whitespace parsing during parsing of the child parser.
 */
class term_parser : public unary_parser {
public:
    /** the constructor.
        @param p child parser.
     */
    term_parser(const parser_object_ptr &p);        
    
    /** Turns off whitespace parsing during the call.
        @param context the current parse context.
        @param parent parent parse tree node; children nodes add themselves to this node.
        @param pos parse position; parsing continues from this position, if successful. 
        @param parse_ws if true, whitespace is parsed between terminals.
        @return true if parsing succeeded, false otherwise.
        @exception left_recursion_success thrown if left recursion is successfully parsed.
     */
    virtual bool parse(parse_context &context, const parse_node_ptr &parent, input_position &pos, bool parse_ws) const;
};


} //namespace parserlib


#endif //PARSERLIB_TERM_PARSER_HPP
