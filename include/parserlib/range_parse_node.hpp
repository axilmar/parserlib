#ifndef PARSERLIB_RANGE_PARSE_NODE_HPP
#define PARSERLIB_RANGE_PARSE_NODE_HPP


#include <cassert>
#include "parse_node.hpp"


namespace parserlib {


    template <class ParseContext, class Symbol>
    class range_parse_node : public parse_node<ParseContext> {
    public:
        range_parse_node(const Symbol& min, const Symbol& max) 
            : m_min(min)
            , m_max(max)
        {
            assert(m_min <= m_max);
        }

        bool parse(ParseContext& pc) const override {
            if (pc.is_valid_iterator()) {
                const auto& token = *pc.get_iterator();
                if (pc.compare(token, m_min) >= 0 && pc.compare(token, m_max) <= 0) {
                    pc.increment_iterator();
                    return true;
                }
            }
            return false;
        }

    private:
        Symbol m_min, m_max;
    };


} //namespace parserlib


#endif //PARSERLIB_RANGE_PARSE_NODE_HPP
