#ifndef PARSERLIB_STRING_PARSE_NODE_HPP
#define PARSERLIB_STRING_PARSE_NODE_HPP


#include <string_view>
#include "parse_node_ptr.hpp"


namespace parserlib {


    template <class ParseContext, class Symbol>
    class string_parse_node : public parse_node<ParseContext> {
    public:
        string_parse_node(const std::basic_string_view<Symbol>& string) 
            : m_string(string)
        {
        }

        bool parse(ParseContext& pc) const override {
            auto itStr = m_string.begin();
            auto itSrc = pc.get_iterator();
            for (;;) {
                if (itStr == m_string.end()) {
                    pc.increment_iterator(m_string.size());
                    return true;
                }
                if (itSrc == pc.get_end_iterator() || pc.compare(*itSrc, *itStr) != 0) {
                    break;
                }
                ++itStr;
                ++itSrc;
            }
            return false;
        }

    private:
        std::basic_string_view<Symbol> m_string;
    };


    template <class ParseContext>
    template <class Symbol>
    parse_node_ptr<ParseContext>::parse_node_ptr(const Symbol* string)
        : m_parse_node(std::make_shared<string_parse_node<ParseContext, Symbol>>(string))
    {
    }


} //namespace parserlib


#endif //PARSERLIB_STRING_PARSE_NODE_HPP
