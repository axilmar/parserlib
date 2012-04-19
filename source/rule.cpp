#include "rule.hpp"


namespace parserlib {


/** constructor from parser expression.
    @param e expression.
 */
rule::rule(const parser_expr &e) : m_expr(e) {
}


/** constructor from character.
    @param c character.
 */
rule::rule(input_char c) : m_expr(c) {
}


/** constructor from null-terminated string.
    @param s null-terminated string.
 */
rule::rule(const char *s) : m_expr(s) {
}


/** constructor from null-terminated string.
    @param s null-terminated string.
 */
rule::rule(const wchar_t *s) : m_expr(s) {
}


/** constructor from rule reference.
    @param r rule.
 */
rule::rule(rule &r) : m_expr(r) {
}


/** invalid constructor (required by some compilers).
    @param r rule.
    @exception std::logic_error.
 */
rule::rule(const rule &r) {
    throw std::logic_error("invalid operation");
}


/** creates a zero-or-more loop out of this rule.
    @return a zero-or-more loop rule.
 */
parser_expr rule::operator *() {
    return *parser_expr(*this);
}


/** creates a one-or-more loop out of this rule.
    @return a one-or-more loop rule.
 */
parser_expr rule::operator +() {
    return +parser_expr(*this);
}


/** creates an optional out of this rule.
    @return an optional rule.
 */
parser_expr rule::operator -() {
    return -parser_expr(*this);
}


/** creates an AND-expression out of this rule.
    @return an AND-expression out of this rule.
 */
parser_expr rule::operator &() {
    return &parser_expr(*this);
}


/** creates a NOT-expression out of this rule.
    @return a NOT-expression out of this rule.
 */
parser_expr rule::operator !() {
    return !parser_expr(*this);
}


/** Parses the rule.
    It does memoization of the parse result, and it also handles left recursion.
    @param context the current parse context.
    @param parent parent parse tree node; children nodes add themselves to this node.
    @param pos parse position; parsing continues from this position, if successful. 
    @param parse_ws if true, whitespace is parsed between terminals.
    @return true if parsing succeeded, false otherwise.
    @exception left_recursion_success thrown if left recursion is successfully parsed.
 */
bool rule::parse(parse_context &context, parse_node &parent, input_position &pos, bool parse_ws) {
    //TODO
    return false;
}

    
} //namespace parserlib
