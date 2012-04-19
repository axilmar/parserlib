#ifndef PARSERLIB_OPTIONAL_PARSER_HPP
#define PARSERLIB_OPTIONAL_PARSER_HPP


#include "unary_parser.hpp"


namespace parserlib {


/** a parser that makes another parser optional.
 */
class optional_parser : public unary_parser {
public:
    /** the constructor.
        @param p child parser.
     */
    optional_parser(const parser_object_ptr &p);        
    
    /** If the child parser fails to parse, the parser state is restored to the one before calling this method.
        @param context the current parse context.
        @param parent parent parse tree node; children nodes add themselves to this node.
        @param pos parse position; parsing continues from this position, if successful. 
        @param parse_ws if true, whitespace is parsed between terminals.
        @return always true.
        @exception left_recursion_success thrown if left recursion is successfully parsed.
     */
    virtual bool parse(parse_context &context, parse_node &parent, input_position &pos, bool parse_ws) const;
};


} //namespace parserlib


#endif //PARSERLIB_OPTIONAL_PARSER_HPP
