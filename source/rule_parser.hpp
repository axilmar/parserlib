#ifndef PARSERLIB_RULE_PARSER_HPP
#define PARSERLIB_RULE_PARSER_HPP


#include "parser_object.hpp"


namespace parserlib {


class rule;


/** a parser object that acts as a wrapper to a rule reference.
    It allows recursive rule definitions.
 */
class rule_parser : public parser_object {
public:
    /** constructor.
        @param r rule.
     */
    rule_parser(rule &r);
    
    /** Passes the call to the rule.
        @param context the current parse context.
        @param parent parent parse tree node; children nodes add themselves to this node.
        @param pos parse position; parsing continues from this position, if successful. 
        @param parse_ws if true, whitespace is parsed between terminals.
        @return true if parsing succeeded, false otherwise.
        @exception left_recursion_success thrown if left recursion is successfully parsed.
     */
    virtual bool parse(parse_context &context, parse_node &parent, input_position &pos, bool parse_ws) const;
    
private:
    //rule
    rule &m_rule;
};


} //namespace parserlib


#endif //PARSERLIB_RULE_PARSER_HPP
