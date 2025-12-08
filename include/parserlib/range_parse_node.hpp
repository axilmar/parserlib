#ifndef PARSERLIB_RANGE_PARSE_NODE_HPP
#define PARSERLIB_RANGE_PARSE_NODE_HPP


#include <cassert>
#include "parse_node.hpp"


namespace parserlib {


    template <class T> 
    class range_parse_node : public interface::parse_node {
    public:
        range_parse_node(const T& min, const T& max) : m_min(min), m_max(max) {
            assert(min <= max);
        }

        bool parse(interface::parse_context& pc) const override {
            if (pc.is_valid_parse_position()) {
                const int current_symbol = pc.get_current_symbol();
                if (pc.compare_symbols(current_symbol, static_cast<int>(m_min)) >= 0 && pc.compare_symbols(current_symbol, static_cast<int>(m_max)) <= 0) {
                    pc.increment_parse_position();
                    return true;
                }
            }
            return false;
        }

    private:
        const T m_min;
        const T m_max;
    };


    template <class T>
    parse_node range(const T& min, const T& max) {
        return interface::create_parse_node<range_parse_node<T>>(min, max);
    }


} //namespace parserlib


#endif //PARSERLIB_RANGE_PARSE_NODE_HPP
