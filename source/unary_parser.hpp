#ifndef PARSERLIB_UNARY_PARSER_HPP
#define PARSERLIB_UNARY_PARSER_HPP


#include "parser_object.hpp"


namespace parserlib {


/** a parser that has only one child.
 */
class unary_parser : public parser_object {
public:
    /** the constructor.
        @param p child parser.
     */
    unary_parser(const parser_object_ptr &p);
    
    /** returns the child parser.
        @return the child parser.
     */
    const parser_object_ptr &parser() const;

private:
    //the child parser
    parser_object_ptr m_parser;
}; 


} //namespace parserlib


#endif //PARSERLIB_UNARY_PARSER_HPP
