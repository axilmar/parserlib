#ifndef PARSERLIB_RANGE_HPP
#define PARSERLIB_RANGE_HPP


#include <cassert>
#include "parse_node.hpp"


namespace parserlib {


    template <class T>
    class range : public parse_node<range<T>> {
    public:
        range(const T& min, const T& max) : m_min(min), m_max(max) {
            assert(min <= max);
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return pc.parse_range(m_min, m_max);
        }

    private:
        const T m_min, m_max;
    };


} //namespace parserlib


#endif //PARSERLIB_RANGE_HPP
