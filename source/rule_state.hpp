#ifndef PARSERLIB_RULE_STATE_HPP
#define PARSERLIB_RULE_STATE_HPP


#include "input_iterator.hpp"


namespace parserlib {


/** represents the state of a rule.
 */
class rule_state {
public:
    ///status.
    enum STATUS {
        ///normal parsing
        NORMAL,
        
        ///reject the rule
        REJECT,
        
        ///accept the rule
        ACCEPT
    };
    
    ///the default constructor.
    rule_state() {}
    
    /** constructor.
        @param it position in the input.
        @param st status.
     */
    rule_state(const input_iterator &it, STATUS st);
    
    /** get the iterator.
        @return the iterator.
     */
    const input_iterator &it() const;
    
    /** get the status.
        @return the status.
     */
    STATUS status() const;

private:
    //iterator
    input_iterator m_it;
    
    //status
    STATUS m_status;
}; 


} //namespace parserlib


#endif //PARSERLIB_RULE_STATE_HPP
