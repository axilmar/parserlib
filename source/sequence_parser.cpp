#include "sequence_parser.hpp"
#include "parse_context.hpp"


namespace parserlib {


/** creates a sequence out of two parsers.
    @param p1 first parser.
    @param p2 second parser.
 */
sequence_parser::sequence_parser(const parser_object_ptr &p1, const parser_object_ptr &p2) :
    composite_parser(p1, p2)
{
}


/** parses successfully if all children parsers parse successfully.
    @param context the current parse context.
    @param parent parent parse tree node; children nodes add themselves to this node.
    @param pos parse position; parsing continues from this position, if successful. 
    @param parse_ws if true, whitespace is parsed between terminals.
    @return true if parsing succeeded, false otherwise.
    @exception left_recursion_success thrown if left recursion is successfully parsed.
 */
bool sequence_parser::parse(parse_context &context, const parse_node_ptr &parent, input_position &pos, bool parse_ws) const {
    //iterators
    parser_object_container::const_iterator it = parsers().begin(), end = parsers().end();

    //parse the first
    const parser_object_ptr &p = *it;
    if (!p->parse(context, parent, pos, parse_ws)) return false;
        
    //parse the rest        
    for(++it; it != end; ++it) {
        //parse whitespace in between
        if (parse_ws) context.parse_whitespace(parent, pos);    
        
        const parser_object_ptr &p = *it;
        if (!p->parse(context, parent, pos, parse_ws)) return false;
    }
    return true;
}


/** creates an optimized sequence out of two other parsers.
    It optimizes the parser tree by not creating a new sequence, if any of the operands is already a sequence.
    @param p1 first parser.
    @param p2 second parser.
    @return a new sequence.
 */
parser_object_ptr create_sequence(const parser_object_ptr &p1, const parser_object_ptr &p2) {
    //get the sequences
    sequence_parser *s1 = dynamic_cast<sequence_parser *>(p1.get());
    sequence_parser *s2 = dynamic_cast<sequence_parser *>(p2.get());
    
    //if both parsers are sequences, return the first one with the second one's added to the first.
    if (s1 && s2) {
        s1->add_parsers(s2->parsers());
        return p1;
    }
    
    //if the first parser is a sequence, add the second to the first and return the first
    if (s1) {
        s1->add_parser(p2);
        return p1;
    }
    
    //if the second parser is a sequence, add the first to the second and return the second
    if (s2) {
        s2->add_parser_to_front(p1);
        return p2;
    }
    
    //no parser was a sequence, so create a new one
    return new sequence_parser(p1, p2);
}


} //namespace parserlib
