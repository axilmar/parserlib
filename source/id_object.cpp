#include "id_object.hpp"


namespace parserlib {


//the id value.
static size_t id_value = 0;


/** the default constructor.
    An id is automatically allocated for the object.
 */
id_object::id_object() {
    alloc_id();
}


/** the copy constructor.
    An id is automatically allocated for the object.
    @param o source object.
 */
id_object::id_object(const id_object &o) {
    alloc_id();
}


/** assignment from object.
    The id member is not copied.
    @param o source object.
    @return reference to this.           
 */
id_object &id_object::operator = (const id_object &o) {
    return *this;
}


/** returns the object's id.
    @return the object's id.
 */
size_t id_object::id() const {
    return m_id;
}


//allocates an id for the object
void id_object::alloc_id() {
    m_id = id_value;
    ++id_value;
}


} //namespace parserlib
