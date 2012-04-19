#include "choice_parser.hpp"
#include "parser_state.hpp"


namespace parserlib {


/** creates a choice out of two parsers.
    @param p1 first parser.
    @param p2 second parser.
 */
choice_parser::choice_parser(const parser_object_ptr &p1, const parser_object_ptr &p2) :
    composite_parser(p1, p2)
{
}


/** parses successfully if one of the children parsers parse successfully.
    @param context the current parse context.
    @param parent parent parse tree node; children nodes add themselves to this node.
    @param pos parse position; parsing continues from this position, if successful. 
    @param parse_ws if true, whitespace is parsed between terminals.
    @return true if parsing succeeded, false otherwise.
    @exception left_recursion_success thrown if left recursion is successfully parsed.
 */
bool choice_parser::parse(parse_context &context, parse_node &parent, input_position &pos, bool parse_ws) const {
    for(parser_object_container::const_iterator it = parsers().begin(), end = parsers().end();
        it != end;
        ++it)
    {
        const parser_object_ptr &p = *it;
        
        //keep the parser state in order to restore it later, if the current parser fails.
        parser_state state(parent, pos);
        
        if (p->parse(context, parent, pos, parse_ws)) return true;
        
        //the current child parser failed, so restore the parser to its previous state,
        //in order to try the next child parser
        state.restore(parent, pos);
    }
    return false;
}


/** creates an optimized choice out of two other parsers.
    It optimizes the parser tree by not creating a new choice, if any of the operands is already a choice.
    @param p1 first parser.
    @param p2 second parser.
    @return a new choice.
 */
parser_object_ptr create_choice(const parser_object_ptr &p1, const parser_object_ptr &p2) {
    //get the choices
    choice_parser *s1 = dynamic_cast<choice_parser *>(p1.get());
    choice_parser *s2 = dynamic_cast<choice_parser *>(p2.get());
    
    //if both parsers are choices, return the first one with the second one's added to the first.
    if (s1 && s2) {
        s1->add_parsers(s2->parsers());
        return p1;
    }
    
    //if the first parser is a choice, add the second to the first and return the first
    if (s1) {
        s1->add_parser(p2);
        return p1;
    }
    
    //if the second parser is a choice, add the first to the second and return the second
    if (s2) {
        s2->add_parser_to_front(p1);
        return p2;
    }
    
    //no parser was a choice, so create a new one
    return new choice_parser(p1, p2);
}


} //namespace parserlib
