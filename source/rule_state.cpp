#include "rule_state.hpp"


namespace parserlib {


/** constructor.
    @param it position in the input.
    @param st status.
 */
rule_state::rule_state(const input_iterator &it, STATUS st) :
    m_it(it), m_status(st)
{
}


/** get the iterator.
    @return the iterator.
 */
const input_iterator &rule_state::it() const {
    return m_it;
}


/** get the status.
    @return the status.
 */
rule_state::STATUS rule_state::status() const {
    return m_status;
}


} //namespace parserlib
