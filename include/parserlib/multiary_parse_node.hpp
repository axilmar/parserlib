#ifndef PARSERLIB_MULTIARY_PARSE_NODE_HPP
#define PARSERLIB_MULTIARY_PARSE_NODE_HPP


#include <tuple>
#include "parse_node.hpp"


namespace parserlib {


    template <class Derived, class... T>
    class multiary_parse_node : public parse_node<Derived> {
    public:
        using tuple_type = std::tuple<T...>;

        multiary_parse_node(const tuple_type& children)
            : m_children(children)
        {
        }

        const tuple_type& children() const {
            return m_children;
        }

    private:
        const tuple_type m_children;
    };


} //namespace parserlib


#endif //PARSERLIB_MULTIARY_PARSE_NODE_HPP
