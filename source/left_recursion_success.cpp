#include "left_recursion_success.hpp"


namespace parserlib {


/** constructor.
    @param rule_id rule id.
 */
left_recursion_success::left_recursion_success(size_t rule_id) :
    m_rule_id(rule_id)
{
}

    
/** returns the rule id.
    @return the rule id.
 */
size_t left_recursion_success::rule_id() const {
    return m_rule_id;
}


} //namespace parserlib
