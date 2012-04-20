#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


//for gcc, lack of rule::rule(const rule &) is reported as an error,
//msvc complains when rule::rule(const rule &) is defined.
#ifdef _MSC_VER
#pragma warning (disable: 4521)
#endif


#include "parser_expr.hpp"
#include "id_object.hpp"
#include "rule_state.hpp"
#include "parse_node_ptr.hpp"


namespace parserlib {


/** Represents a rule of the grammar.
 */
class rule : public id_object {
public:
    /** constructor from parser expression.
        @param e expression.
     */
    rule(const parser_expr &e);
    
    /** constructor from character.
        @param c character.
     */
    rule(input_char c);
    
    /** constructor from null-terminated string.
        @param s null-terminated string.
     */
    rule(const char *s);
    
    /** constructor from null-terminated string.
        @param s null-terminated string.
     */
    rule(const wchar_t *s);
    
    /** constructor from rule reference.
        @param r rule.
     */
    rule(rule &r);
    
    /** invalid constructor (required by some compilers).
        @param r rule.
        @exception std::logic_error.
     */
    rule(const rule &r);

    /** creates a zero-or-more loop out of this rule.
        @return a zero-or-more loop rule.
     */
    parser_expr operator *();

    /** creates a one-or-more loop out of this rule.
        @return a one-or-more loop rule.
     */
    parser_expr operator +();

    /** creates an optional out of this rule.
        @return an optional rule.
     */
    parser_expr operator -();

    /** creates an AND-expression out of this rule.
        @return an AND-expression out of this rule.
     */
    parser_expr operator &();

    /** creates a NOT-expression out of this rule.
        @return a NOT-expression out of this rule.
     */
    parser_expr operator !();
    
    /** Parses the rule.
        It does memoization of the parse result, and it also handles left recursion.
        @param context the current parse context.
        @param parent parent parse tree node; children nodes add themselves to this node.
        @param pos parse position; parsing continues from this position, if successful. 
        @param parse_ws if true, whitespace is parsed between terminals.
        @return true if parsing succeeded, false otherwise.
        @exception left_recursion_success thrown if left recursion is successfully parsed.
     */
    bool parse(parse_context &context, const parse_node_ptr &parent, input_position &pos, bool parse_ws);
    
    /** Parses the rule.
        It does memoization of the parse result, and it also handles left recursion.
        @param context the current parse context.
        @param pos parse position; parsing continues from this position, if successful. 
        @param parse_ws if true, whitespace is parsed between terminals.
        @param root the result root node of the parse tree.
        @return true if parsing succeeded, false otherwise.
     */
    bool parse(parse_context &context, input_position &pos, bool parse_ws, parse_node_ptr &root);
    
private:
    //wrapper over a parser expression
    parser_expr m_expr;    

    //internal parse with a different status
    bool internal_parse(parse_context &context, const parse_node_ptr &parent, input_position &pos, bool parse_ws, rule_state::STATUS status);
}; 


} //namespace parserlib


#endif //PARSERLIB_RULE_HPP
