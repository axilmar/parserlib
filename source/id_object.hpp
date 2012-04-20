#ifndef PARSERLIB_ID_OBJECT_HPP
#define PARSERLIB_ID_OBJECT_HPP


namespace parserlib {


/** Base class for objects with an id.
 */
class id_object {
public:
    /** the default constructor.
        An id is automatically allocated for the object.
     */
    id_object();

    /** the copy constructor.
        An id is automatically allocated for the object.
        @param o source object.
     */
    id_object(const id_object &o);
    
    /** assignment from object.
        The id member is not copied.
        @param o source object.
        @return reference to this.           
     */
    id_object &operator = (const id_object &o);
    
    /** returns the object's id.
        @return the object's id.
     */
    size_t id() const;

private:
    //the object's id
    size_t m_id;
    
    //allocates an id for the object
    void alloc_id();    
}; 


} //namespace parserlib


#endif //PARSERLIB_ID_OBJECT_HPP
