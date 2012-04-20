#ifndef PARSERLIB_PARSE_CONTEXT_HPP
#define PARSERLIB_PARSE_CONTEXT_HPP


#include "parse_node.hpp"
#include "rule_state.hpp"


namespace parserlib {


/** parse context.
 */
class parse_context {
public:
    /** the default constructor.
        @param buf input buffer.
        @param ws whitespace rule.
     */
    parse_context(input_buffer &buf, rule &ws);
    
    /** returns the input end.
        @return the input end.
     */
    const input_iterator &input_end() const;
    
    /** checks if the given position does not point to the end of the input buffer.
        @param pos position.
        @return true if the given position does not point to the end of the input buffer, false otherwise.
     */
    bool valid_position(const input_position &pos) const;
    
    /** checks if the given position has reached the end of the input.
        @param pos position.
        @return true if the end has been reached, false otherwise.
     */
    bool end_position(const input_position &pos) const;
    
    /** returns the error position.
        @return the error position.
     */
    const input_position &error_position() const;
    
    /** sets the error position, but only if the new position is higher than the last error position.
        This allows the parser to report the most further error.
        @param pos the new error position.
     */
    void set_error_position(const input_position &pos);
    
    /** parse whitespace.
        @param parent parent parse tree node; children nodes add themselves to this node.
        @param pos parse position; parsing continues from this position, if successful. 
     */
    void parse_whitespace(const parse_node_ptr &parent, input_position &pos);
    
    /** returns the current rule state for a rule.
        @param rule_id id of the rule to get the state of.
        @param it current position.
        @param st return state.
        @return true for left recursion, false otherwise.
     */
    bool get_rule_state(size_t rule_id, const input_iterator &it, rule_state &st) const;

    /** push a rule state for a rule.
        @param rule_id id of the rule to push a state for.
        @param st state.
     */
    void push_rule_state(size_t rule_id, const rule_state &st);
    
    /** pops a rule state for a rule.
        @param rule_id id of the rule to pop a state for.
     */
    void pop_rule_state(size_t rule_id);
    
    /** recalls a parse tree.
        @param rule_id rule id.
        @param pos current position.
        @param node result node.
        @return true if the recalling succeeded, false otherwise.
     */
    bool recall_parse_tree(size_t rule_id, const input_position &pos, parse_node_ptr &node) const;
    
    /** memoizes a parse tree.
        @param node the parse node to memoize.
     */
    void memoize_parse_tree(parse_node_ptr &node);

private:
    //type of rule state container
    typedef std::vector<rule_state> rule_state_stack;
    
    //type of container of rule to rule state container.
    typedef std::vector<rule_state_stack> rule_state_container;

    //iterator to end of input buffer.
    input_iterator m_input_end;
    
    //the error position
    input_position m_error_position;
    
    //the whitespace rule
    rule &m_whitespace;
    
    //rule states
    rule_state_container m_rule_states;
}; 


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_HPP
