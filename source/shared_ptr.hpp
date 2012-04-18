#ifndef PARSERLIB_SHARED_PTR_HPP
#define PARSERLIB_SHARED_PTR_HPP


#include <cassert>


namespace parserlib {


/** A pointer that uses reference counting to manage an object's lifetime.

    The library offers its own shared pointer implementation, for two reasons:
    
    - the standard c++ shared pointer implementation is not intrusive.
    - not all c++ compilers support the standard c++ shared pointers, at the time this is written.
    
    The pointer is not thread-safe.
    
    Non-intrusive shared pointers have the following problems:
    
    - initialization requires declaring the pointer twice, due to explicit constructor.
    - unboxing the pointer and then boxing it creates two shared pointers.
    - using shared pointers to an object from inside a method is impossible, unless 
      the class inherits from 'enabled_shared_ptr_from_this'.
    - non-intrusive shared pointers do not have all the pointer semantics.
      
    Boost pointers could have been used, but then the boost dependency would be required to be managed.

    @param T type of object to point to; it must provide an interface to manage the
        reference count, similar to class shared_object.
 */
template <class T> class shared_ptr {
public:
    ///null pointer constructor.
    shared_ptr() : m_object(0) {
    }
    
    /** constructor from native pointer.
        If the pointer is not null, the object's reference count is incremented.
        @param obj native pointer to object.
     */
    shared_ptr(T *obj) : m_object(obj) {
        inc_ref(m_object);
    }
    
    /** copy constructor.
        If the pointer is not null, the object's reference count is incremented.
        @param obj native pointer to object.
     */
    shared_ptr(const shared_ptr<T> &obj) : m_object(obj) {
        inc_ref(m_object);
    }

    /** destructor.
        If the pointer is not null, the object's reference count is decremented.
        If the reference count's value becomes zero, the object is deleted.
     */
    ~shared_ptr() {
        dec_ref(m_object);
    }
    
    /** assignment from native pointer.
        The new object's reference count is incremented,
        the old object's reference count is decremented.
        If the old object's reference count becomes zero, the object is deleted.
        @param obj native pointer to object.
        @return reference to this.
     */
    shared_ptr<T> &operator = (T *obj) {
        set(obj);
        return *this;
    }
    
    /** the assignment operator.
        The new object's reference count is incremented,
        the old object's reference count is decremented.
        If the old object's reference count becomes zero, the object is deleted.
        @param obj native pointer to object.
        @return reference to this.
     */
    shared_ptr<T> &operator = (const shared_ptr<T> &obj) {
        set(obj.m_object);
        return *this;
    }
    
    /** returns the native pointer.
        @return the native pointer.
     */
    T *get() const {
        return m_object;
    }
    
    /** automatic conversion to native pointer.
        @return the native pointer.
     */
    operator T* () const {
        return m_object;
    }

    /** member access.
        An assertion makes certain the pointer is not null prior to being returned.
        @return the native pointer.
     */
    T *operator ->() const {
        assert(m_object);
        return m_object;
    }

private:
    //the pointed object.
    T *m_object;
    
    //increment the reference count of the object
    static void inc_ref(T *obj) {
        if (obj) obj->inc_ref();
    }
    
    //decrement the reference count of the object and delete it
    //if the reference count reaches 0
    static void dec_ref(T *obj) {
        if (obj && obj->dec_ref() == 0) {
            delete obj;
        }
    }
    
    //set the pointer by first incrementing the new object's reference count,
    //then decrementing the old object's reference count
    void set(T *obj) {
        inc_ref(obj);
        dec_ref(m_object);
        m_object = obj;
    }
}; 


} //namespace parserlib


#endif //PARSERLIB_SHARED_PTR_HPP
