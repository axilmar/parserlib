#include "rule.hpp"
#include "parse_context.hpp"
#include "parser_state.hpp"
#include "left_recursion_success.hpp"


namespace parserlib {


/** constructor from parser expression.
    @param e expression.
 */
rule::rule(const parser_expr &e) : m_expr(e), m_name(0) {
}


/** constructor from character.
    @param c character.
 */
rule::rule(input_char c) : m_expr(c), m_name(0) {
}


/** constructor from null-terminated string.
    @param s null-terminated string.
 */
rule::rule(const char *s) : m_expr(s), m_name(0) {
}


/** constructor from null-terminated string.
    @param s null-terminated string.
 */
rule::rule(const wchar_t *s) : m_expr(s), m_name(0) {
}


/** constructor from rule reference.
    @param r rule.
 */
rule::rule(rule &r) : m_expr(r), m_name(0) {
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
bool rule::parse(parse_context &context, const parse_node_ptr &parent, input_position &pos, bool parse_ws) {    
    //if a previous result exists for this rule at this position, reuse it
    parse_node_ptr pn;
    if (context.recall_parse_tree(id(), pos, pn)) {
        parent->add_subnode(pn);
        return true;
    }
    
    //get the current rule state and left recursion status
    rule_state st;
    bool left_rec = context.get_rule_state(id(), pos.it(), st);
    
    bool success;
    
    //keep the begin position
    input_iterator begin = pos.it();
    
    //handle the current rule's status
    switch (st.status()) {
        //normal parse
        case rule_state::NORMAL:
            if (left_rec) {
                //first try to parse the rule by rejecting it, so alternative branches are examined                
                success = internal_parse(context, parent, pos, parse_ws, rule_state::REJECT);
                
                //if the first try is successful, try accepting the rule,
                //so other elements of the sequence are parsed
                if (success) {
                    //loop until no more parsing can be done
                    for(;;) {
                        parser_state st(parent, pos);
                        if (!internal_parse(context, parent, pos, parse_ws, rule_state::ACCEPT)) {
                            st.restore(parent, pos);
                            break;
                        }
                    }
                    
                    //since the left recursion was resolved successfully,
                    //return via a non-local exit 
                    throw left_recursion_success(id());
                }
            }
            else {
                //try normal parsing
                try {
                    pn = new parse_node(*this, pos);                
                    success = internal_parse(context, pn, pos, parse_ws, rule_state::NORMAL);
                    goto BUILD_PARSE_TREE;
                }
                
                //since left recursions may be mutual, 
                //we must test which rule's left recursion
                //was ended successfully
                catch (const left_recursion_success &ex) {                
                    if (ex.rule_id() == id()) {
                        success = true;
                        goto BUILD_PARSE_TREE;
                    }
                    else {
                        throw;
                    }
                }
            }
            break;
            
        //reject the left recursive rule
        case rule_state::REJECT:
            if (left_rec) {
                return false;
            }
            else {
                pn = new parse_node(*this, pos);                
                success = internal_parse(context, pn, pos, parse_ws, rule_state::NORMAL);
                goto BUILD_PARSE_TREE;
            }
            break;
            
        //accept the left recursive rule
        case rule_state::ACCEPT:
            if (left_rec) {
                return true;
            }
            else {
                pn = new parse_node(*this, pos);                
                success = internal_parse(context, pn, pos, parse_ws, rule_state::NORMAL);
                goto BUILD_PARSE_TREE;
            }
            break;
    }
    
    return success;

    BUILD_PARSE_TREE:
    if (success && pos.it() > begin) {
        pn->set_end_position(pos);
        parent->add_subnode(pn);
        context.memoize_parse_tree(pn);
    }                    
    return success;
}

   
/** Parses the rule.
    It does memoization of the parse result, and it also handles left recursion.
    @param context the current parse context.
    @param pos parse position; parsing continues from this position, if successful. 
    @param parse_ws if true, whitespace is parsed between terminals.
    @param root the result root node of the parse tree.
    @return true if parsing succeeded, false otherwise.
 */
bool rule::parse(parse_context &context, input_position &pos, bool parse_ws, parse_node_ptr &root) {
    //recall the parse tree 
    if (context.recall_parse_tree(id(), pos, root)) {
        return true;
    }

    //create a new parse node for this rule
    root = new parse_node(*this, pos);
    
    //parse initial whitespace
    if (parse_ws) context.parse_whitespace(root, pos);    
    
    input_iterator begin = pos.it();
    
    //parse with normal state
    bool success = internal_parse(context, root, pos, parse_ws, rule_state::NORMAL);
    
    //on success, memoize the result,
    //parse the remaining whitespace 
    //and reevaluate the success status by checking
    //if the whole input is consumed
    if (success && pos.it() > begin) {
        context.memoize_parse_tree(root);
        context.parse_whitespace(root, pos);
        success = context.end_position(pos);
    }
    
    return success;
}


/** returns the rule's name.
    @return the rule's name.
 */
const char *rule::name() const {
    return m_name;
}


/** sets the rule's name.
    @param n name.
 */
void rule::set_name(const char *name) {
    m_name = name;
}


//internal parse with a different status
bool rule::internal_parse(parse_context &context, const parse_node_ptr &parent, input_position &pos, bool parse_ws, rule_state::STATUS status) {
    //create a new state for the rule
    context.push_rule_state(id(), rule_state(pos.it(), status));
    
    bool success;
    try {
        //try to parse the internal expression
        success = m_expr.parser()->parse(context, parent, pos, parse_ws);
    }
    
    //catch left recursion exception in order to pop the rule state
    catch (const left_recursion_success &) {
        context.pop_rule_state(id());
        throw;
    }

    //pop the rule state    
    context.pop_rule_state(id());
    return success;
}


} //namespace parserlib
