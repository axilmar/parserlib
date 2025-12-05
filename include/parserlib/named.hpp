#ifndef PARSERLIB_NAMED_HPP
#define PARSERLIB_NAMED_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class T>
    auto named(const std::string& name, T&& parse_node) {
        auto result = make_parse_node(parse_node);
        result.set_name(name);
        return result;
    }


} //namespace parserlib


#endif //PARSERLIB_NAMED_HPP
