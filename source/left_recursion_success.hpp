#ifndef PARSERLIB_LEFT_RECURSION_SUCCESS_HPP
#define PARSERLIB_LEFT_RECURSION_SUCCESS_HPP


namespace parserlib {


/** reporting successful parsing of left recursion is done via this exception,
    because it makes the design much much simpler.
 */
class left_recursion_success {
public:
    /** constructor.
        @param rule_id rule id.
     */
    left_recursion_success(size_t rule_id);
    
    /** returns the rule id.
        @return the rule id.
     */
    size_t rule_id() const;

private:
    //rule id
    size_t m_rule_id;
}; 


} //namespace parserlib


#endif //PARSERLIB_LEFT_RECURSION_SUCCESS_HPP
