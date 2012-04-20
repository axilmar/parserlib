#ifndef PARSERLIB_PARSE_NODE_PTR_HPP
#define PARSERLIB_PARSE_NODE_PTR_HPP


#include "shared_ptr.hpp"


namespace parserlib {


class parse_node;


///type of pointer to parse node.
typedef shared_ptr<parse_node> parse_node_ptr;


} //namespace parserlib


#endif //PARSERLIB_PARSE_NODE_PTR_HPP
