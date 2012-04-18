#include <cassert>
#include "shared_object.hpp"


namespace parserlib {


/** the default constructor.
    The reference counter is initialized to 0.
 */
shared_object::shared_object() : m_ref_count(0) {
}


/** the copy constructor.
    The reference counter is initialized to 0.
    @param src source object.
 */
shared_object::shared_object(const shared_object &src) : m_ref_count(0) {
}


/** the destructor.
    An assertion makes sure the reference count is 0.
 */
shared_object::~shared_object() {
    assert(m_ref_count == 0);
}


/** the assignment operator.
    The reference count is not copied.
    @param src source object.
    @return reference to this.
 */
shared_object &shared_object::operator = (const shared_object &src) {
    return *this;
}


/** returns the object's reference count.
    @return the object's reference count.
 */
size_t shared_object::get_ref() const {
    return m_ref_count;
}


/** increments the object's reference count.
    An assertion makes sure the reference count is not wrapped around to 0.
    @return the value of the reference count after the operation.
 */
size_t shared_object::inc_ref() {
    ++m_ref_count;
    assert(m_ref_count != 0);
    return m_ref_count;
}


/** decrements the object's reference count.
    An assertion makes sure the reference count is not 0 before the operation.
    @return the value of the reference count after the operation.
 */
size_t shared_object::dec_ref() {
    assert(m_ref_count != 0);
    return --m_ref_count;
}


} //namespace parserlib
