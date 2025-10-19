#ifndef PARSERLIB_WRAPPER_PARSER_NODE_HPP
#define PARSERLIB_WRAPPER_PARSER_NODE_HPP


#include "parser_node.hpp"


namespace parserlib {



    template <class ParseContext> class wrapper_parser_node_interface : public parser_node<wrapper_parser_node_interface<ParseContext>> {
    public:
        virtual ~wrapper_parser_node_interface() {
        }

        virtual bool parse(ParseContext& pc) const = 0;
    };


    template <class ParseContext, class Parser> class wrapper_parser_node_implementation : public wrapper_parser_node_interface<ParseContext> {
    public:
        wrapper_parser_node_implementation(const Parser& parser) : m_parser(parser) {
        }

        bool parse(ParseContext& pc) const override {
            return m_parser.parse(pc);
        }

    private:
        Parser m_parser;
    };


} //namespace parserlib


#endif // PARSERLIB_WRAPPER_PARSER_NODE_HPP
