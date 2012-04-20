#include "parse_context.hpp"
#include "rule.hpp"


namespace parserlib {


/** the default constructor.
    @param buf input buffer.
    @param ws whitespace rule.
 */
parse_context::parse_context(input_buffer &buf, rule &ws) : 
    m_input_end(buf.end()), m_error_position(buf), m_whitespace(ws)
{
    m_rule_states.reserve(256);
}

    
/** returns the input end.
    @return the input end.
 */
const input_iterator &parse_context::input_end() const {
    return m_input_end;
}


/** checks if the given position does not point to the end of the input buffer.
    @param pos position.
    @return true if the given position does not point to the end of the input buffer, false otherwise.
 */
bool parse_context::valid_position(const input_position &pos) const {
    return pos.it() != m_input_end;
}


/** checks if the given position has reached the end of the input.
    @param pos position.
    @return true if the end has been reached, false otherwise.
 */
bool parse_context::end_position(const input_position &pos) const {
    return pos.it() == m_input_end;
}


/** returns the error position.
    @return the error position.
 */
const input_position &parse_context::error_position() const {
    return m_error_position;
}


/** sets the error position, but only if the new position is higher than the last error position.
    This allows the parser to report the most further error.
    @param pos the new error position.
 */
void parse_context::set_error_position(const input_position &pos) {
    if (pos > m_error_position) {
        m_error_position = pos;
    }
}


/** parse whitespace.
    @param parent parent parse tree node; children nodes add themselves to this node.
    @param pos parse position; parsing continues from this position, if successful. 
 */
void parse_context::parse_whitespace(const parse_node_ptr &parent, input_position &pos) {
    m_whitespace.parse(*this, parent, pos, false);
}


/** returns the current rule state for a rule.
    @param rule_id id of the rule to get the state of.
    @param it current position.
    @param st return state.
    @return true for left recursion, false otherwise.
 */
bool parse_context::get_rule_state(size_t rule_id, const input_iterator &it, rule_state &st) const {
    //find the top state
    if (rule_id < m_rule_states.size() && !m_rule_states[rule_id].empty()) {
        //get the most recent state
        st = m_rule_states[rule_id].back();
        
        //left recursion exists when the previous state's position and the current state's position are equal
        return it == st.it();
    }
    
    //no previous rule state existed; return the normal state without left recursion
    st = rule_state(it, rule_state::NORMAL);
    return false;
}


/** push a rule state for a rule.
    @param rule_id id of the rule to push a state for.
    @param st state.
 */
void parse_context::push_rule_state(size_t rule_id, const rule_state &st) {
    if (rule_id >= m_rule_states.size()) {
        m_rule_states.resize(rule_id + 1);
    }
    m_rule_states[rule_id].push_back(st);
}


/** pops a rule state for a rule.
    @param rule_id id of the rule to pop a state for.
 */
void parse_context::pop_rule_state(size_t rule_id) {
    m_rule_states[rule_id].pop_back();
}


/** recalls a parse tree.
    @param rule_id rule id.
    @param pos current position.
    @param node result node.
    @return true if the recalling succeeded, false otherwise.
 */
bool parse_context::recall_parse_tree(size_t rule_id, const input_position &pos, parse_node_ptr &node) const {
    //TODO
    return false;
}


/** memoizes a parse tree.
    @param node the parse node to memoize.
 */
void parse_context::memoize_parse_tree(parse_node_ptr &node) {
    //TODO
}


} //namespace parserlib
