#ifndef PARSERLIB_BOOL_PARSE_NODE_HPP
#define PARSERLIB_BOOL_PARSE_NODE_HPP


#include "false_parse_node.hpp"
#include "true_parse_node.hpp"


namespace parserlib {


    template <class ParseContext>
    parse_node_ptr<ParseContext>::parse_node_ptr(bool result)
        : m_parse_node(
            result 
            ? std::static_pointer_cast<parse_node<ParseContext>>(std::make_shared<true_parse_node<ParseContext>>())
            : std::static_pointer_cast<parse_node<ParseContext>>(std::make_shared<false_parse_node<ParseContext>>())
        )
    {
    }


} //namespace parserlib


#endif //PARSERLIB_BOOL_PARSE_NODE_HPP
