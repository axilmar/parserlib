#ifndef PARSERLIB_SEQUENCE_PARSER_HPP
#define PARSERLIB_SEQUENCE_PARSER_HPP


#include "composite_parser.hpp"


namespace parserlib {


/** a composite parser that parses successfully if all children parsers parse successfully.
 */
class sequence_parser : public composite_parser {
public:
    /** creates a sequence out of two parsers.
        @param p1 first parser.
        @param p2 second parser.
     */
    sequence_parser(const parser_object_ptr &p1, const parser_object_ptr &p2);
    
    /** parses successfully if all children parsers parse successfully.
        @param context the current parse context.
        @param parent parent parse tree node; children nodes add themselves to this node.
        @param pos parse position; parsing continues from this position, if successful. 
        @param parse_ws if true, whitespace is parsed between terminals.
        @return true if parsing succeeded, false otherwise.
        @exception left_recursion_success thrown if left recursion is successfully parsed.
     */
    virtual bool parse(parse_context &context, parse_node &parent, input_position &pos, bool parse_ws) const;
}; 


/** creates an optimized sequence out of two other parsers.
    It optimizes the parser tree by not creating a new sequence, if any of the operands is already a sequence.
    @param p1 first parser.
    @param p2 second parser.
    @return a new sequence.
 */
parser_object_ptr create_sequence(const parser_object_ptr &p1, const parser_object_ptr &p2); 


} //namespace parserlib


#endif //PARSERLIB_SEQUENCE_PARSER_HPP
