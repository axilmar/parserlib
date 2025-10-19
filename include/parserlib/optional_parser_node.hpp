#ifndef PARSERLIB_OPTIONAL_PARSER_NODE_HPP
#define PARSERLIB_OPTIONAL_PARSER_NODE_HPP


#include "parser_node.hpp"


namespace parserlib {


    template <class Parser> class optional_parser_node : public parser_node<optional_parser_node<Parser>> {
    public:
        optional_parser_node(const Parser& parser) : m_parser(parser) {
        }

        template <class ParseContext> bool parse(ParseContext& pc) const {
            m_parser.parse(pc);
            return true;
        }

    private:
        Parser m_parser;
    };


    template <class Parser> optional_parser_node<Parser> parser_node<Parser>::operator -() const {
        return optional_parser_node<Parser>(parser(static_cast<const Parser&>(*this)));
    }


} //namespace parserlib


#endif // PARSERLIB_OPTIONAL_PARSER_NODE_HPP
