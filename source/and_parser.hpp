#ifndef PARSERLIB_AND_PARSER_HPP
#define PARSERLIB_AND_PARSER_HPP


#include "unary_parser.hpp"


namespace parserlib {


/** a parser that corresponds to the logical AND operator.
 */
class and_parser : public unary_parser {
public:
    /** the constructor.
        @param p child parser.
     */
    and_parser(const parser_object_ptr &p);        
    
    /** If the child parser returns true, it returns true, otherwise it returns false.
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


#endif //PARSERLIB_AND_PARSER_HPP
