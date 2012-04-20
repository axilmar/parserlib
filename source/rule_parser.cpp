#include "rule_parser.hpp"
#include "rule.hpp"


namespace parserlib {


/** constructor.
    @param r rule.
 */
rule_parser::rule_parser(rule &r) : m_rule(r) {
}


/** Passes the call to the rule.
    @param context the current parse context.
    @param parent parent parse tree node; children nodes add themselves to this node.
    @param pos parse position; parsing continues from this position, if successful. 
    @param parse_ws if true, whitespace is parsed between terminals.
    @return true if parsing succeeded, false otherwise.
    @exception left_recursion_success thrown if left recursion is successfully parsed.
 */
bool rule_parser::parse(parse_context &context, const parse_node_ptr &parent, input_position &pos, bool parse_ws) const {
    return m_rule.parse(context, parent, pos, parse_ws);
}


} //namespace parserlib
