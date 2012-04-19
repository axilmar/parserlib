#ifndef PARSERLIB_CHOICE_PARSER_HPP
#define PARSERLIB_CHOICE_PARSER_HPP


#include "composite_parser.hpp"


namespace parserlib {


/** a composite parser that parses successfully if one of the children parsers parse successfully.
 */
class choice_parser : public composite_parser {
public:
    /** creates a choice out of two parsers.
        @param p1 first parser.
        @param p2 second parser.
     */
    choice_parser(const parser_object_ptr &p1, const parser_object_ptr &p2);
    
    /** parses successfully if one of the children parsers parse successfully.
        @param context the current parse context.
        @param parent parent parse tree node; children nodes add themselves to this node.
        @param pos parse position; parsing continues from this position, if successful. 
        @param parse_ws if true, whitespace is parsed between terminals.
        @return true if parsing succeeded, false otherwise.
        @exception left_recursion_success thrown if left recursion is successfully parsed.
     */
    virtual bool parse(parse_context &context, parse_node &parent, input_position &pos, bool parse_ws) const;
}; 


/** creates an optimized choice out of two other parsers.
    It optimizes the parser tree by not creating a new choice, if any of the operands is already a choice.
    @param p1 first parser.
    @param p2 second parser.
    @return a new choice.
 */
parser_object_ptr create_choice(const parser_object_ptr &p1, const parser_object_ptr &p2); 


} //namespace parserlib


#endif //PARSERLIB_CHOICE_PARSER_HPP
