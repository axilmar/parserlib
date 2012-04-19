#include "unary_parser.hpp"


namespace parserlib {


/** the constructor.
    @param p child parser.
 */
unary_parser::unary_parser(const parser_object_ptr &p) : m_parser(p) {
}


/** returns the child parser.
    @return the child parser.
 */
const parser_object_ptr &unary_parser::parser() const {
    return m_parser;
}


} //namespace parserlib
