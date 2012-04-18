#ifndef PARSERLIB_SHARED_OBJECT_HPP
#define PARSERLIB_SHARED_OBJECT_HPP


namespace parserlib {


/** base class for reference-counted objects.

    Shared pointers of the parserlib library are like boost::intrusive_ptrs,
    i.e. the target object must have an embedded reference counter. This class
    provides the reference counter which is then used by the shared pointer.
    
    Embedding the reference counter into an object makes memory management faster
    and simpler.
    
    The reference counter provided by this object is not thread safe.
 */
class shared_object {
public:
    /** the default constructor.
        The reference counter is initialized to 0.
     */
    shared_object();
    
    /** the copy constructor.
        The reference counter is initialized to 0.
        @param src source object.
     */
    shared_object(const shared_object &src);
    
    /** the destructor.
        An assertion makes sure the reference count is 0.
     */
    ~shared_object();
    
    /** the assignment operator.
        The reference count is not copied.
        @param src source object.
        @return reference to this.
     */
    shared_object &operator = (const shared_object &src);
    
    /** returns the object's reference count.
        @return the object's reference count.
     */
    size_t get_ref() const;
    
    /** increments the object's reference count.
        An assertion makes sure the reference count is not wrapped around to 0.
        @return the value of the reference count after the operation.
     */
    size_t inc_ref();
    
    /** decrements the object's reference count.
        An assertion makes sure the reference count is not 0 before the operation.
        @return the value of the reference count after the operation.
     */
    size_t dec_ref();

private:
    //reference counter
    size_t m_ref_count;
}; 


} //namespace parserlib


#endif //PARSERLIB_SHARED_OBJECT_HPP
