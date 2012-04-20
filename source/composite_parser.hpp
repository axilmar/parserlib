#ifndef PARSERLIB_COMPOSITE_PARSER_HPP
#define PARSERLIB_COMPOSITE_PARSER_HPP


#include <vector>
#include "parser_object.hpp"


namespace parserlib {


///parser object container.
typedef std::vector<parser_object_ptr> parser_object_container;


/** base class for parsers with children.
 */
class composite_parser : public parser_object {
public:
    /** adds the two given parsers to the list of parsers.
        @param p1 first parser.
        @param p2 second parser.
     */
    composite_parser(const parser_object_ptr &p1, const parser_object_ptr &p2);

    /** returns the container of children.
        @return the container of children.
     */
    const parser_object_container &parsers() const;
    
    /** adds a child parser.
        @param p pointer to the child parser.
     */
    void add_parser(const parser_object_ptr &p);
    
    /** adds child parsers.
        @param c container with parsers.
     */
    void add_parsers(const parser_object_container &c);

    /** adds a child parser to the front of the container.
        @param p pointer to the child parser.
     */
    void add_parser_to_front(const parser_object_ptr &p);
    
private:
    //children parsers
    parser_object_container m_parsers;
}; 


} //namespace parserlib


#endif //PARSERLIB_COMPOSITE_PARSER_HPP
