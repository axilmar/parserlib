#include "composite_parser.hpp"


namespace parserlib {


/** adds the two given parsers to the list of parsers.
    @param p1 first parser.
    @param p2 second parser.
 */
composite_parser::composite_parser(const parser_object_ptr &p1, const parser_object_ptr &p2) {
    m_parsers.push_back(p1);
    m_parsers.push_back(p2);
}


/** returns the container of children.
    @return the container of children.
 */
const parser_object_container &composite_parser::parsers() const {
    return m_parsers;
}


/** adds a child parser.
    @param p pointer to the child parser.
 */
void composite_parser::add_parser(const parser_object_ptr &p) {
    m_parsers.push_back(p);
}


/** adds child parsers.
    @param c container with parsers.
 */
void composite_parser::add_parsers(const parser_object_container &c) {
    assert(&c != &m_parsers);
    m_parsers.insert(m_parsers.end(), c.begin(), c.end());
}


/** adds a child parser to the front of the container.
    @param p pointer to the child parser.
 */
void composite_parser::add_parser_to_front(const parser_object_ptr &p) {
    m_parsers.insert(m_parsers.begin(), p);    
}


} //namespace parserlib
