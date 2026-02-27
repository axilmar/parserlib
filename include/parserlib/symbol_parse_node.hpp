#ifndef PARSERLIB_SYMBOL_PARSE_NODE_HPP
#define PARSERLIB_SYMBOL_PARSE_NODE_HPP


#include "parse_node_ptr.hpp"


namespace parserlib {


    template <class ParseContext, class Symbol>
    class symbol_parse_node : public parse_node<ParseContext> {
    public:
        symbol_parse_node(const Symbol& symbol) 
            : m_symbol(symbol)
        {
        }

        bool parse(ParseContext& pc) const override {
            if (pc.is_valid_iterator()) {
                const auto& token = *pc.get_iterator();
                if (pc.compare(token, m_symbol) == 0) {
                    pc.increment_iterator();
                    return true;
                }
            }
            return false;
        }

    private:
        Symbol m_symbol;
    };


    template <class ParseContext>
    template <class Symbol>
    parse_node_ptr<ParseContext>::parse_node_ptr(const Symbol& symbol)
        : m_parse_node(std::make_shared<symbol_parse_node<ParseContext, Symbol>>(symbol))
    {
    }


} //namespace parserlib


#endif //PARSERLIB_SYMBOL_PARSE_NODE_HPP
