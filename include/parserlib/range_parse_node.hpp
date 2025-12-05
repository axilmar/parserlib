#ifndef PARSERLIB_RANGE_PARSE_NODE_HPP
#define PARSERLIB_RANGE_PARSE_NODE_HPP


#include <cassert>
#include "parse_node.hpp"


namespace parserlib {


    template <class T>
    class range_parse_node : public parse_node<range_parse_node<T>> {
    public:
        range_parse_node(const T& min, const T& max)
            : m_min(min)
            , m_max(max)
        {
            assert(min <= max);
        }

        bool parse(parse_context_interface& pc) const {
            return pc.parse_range(static_cast<int>(m_min), static_cast<int>(m_max));
        }

        const T& min() const {
            return m_min;
        }

        const T& max() const {
            return m_max;
        }

    private:
        const T m_min, m_max;
    };


    template <class T>
    auto range(const T& min, const T& max) {
        return range_parse_node<T>(min, max);
    }


} //namespace parserlib


#endif //PARSERLIB_RANGE_PARSE_NODE_HPP
