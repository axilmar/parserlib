#ifndef PARSERLIB_SET_PARSE_NODE_HPP
#define PARSERLIB_SET_PARSE_NODE_HPP


#include <string_view>
#include <vector>
#include <algorithm>
#include "parse_node.hpp"


namespace parserlib {


    template <class ParseContext, class Symbol>
    class set_parse_node : public parse_node<ParseContext> {
    public:
        set_parse_node(const std::basic_string_view<Symbol>& set)
            : m_set(set.begin(), set.end())
        {
            std::sort(m_set.begin(), m_set.end());
        }

        bool parse(ParseContext& pc) const override {
            if (pc.is_valid_iterator()) {
                const auto& token = *pc.get_iterator();
                auto it = std::upper_bound(m_set.begin(), m_set.end(), token, [&](const auto& a,const auto& b) {
                    return pc.compare(a, b) < 0;
                });
                if (it != m_set.begin()) {
                    --it;
                    if (pc.compare(token, *it) == 0) {
                        pc.increment_iterator();
                        return true;
                    }
                }
            }
            return false;
        }

    private:
        std::vector<Symbol> m_set;
    };


} //namespace parserlib


#endif //PARSERLIB_SET_PARSE_NODE_HPP
