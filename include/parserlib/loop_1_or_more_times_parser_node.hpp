#ifndef PARSERLIB_LOOP_1_OR_MORE_TIMES_PARSER_NODE_HPP
#define PARSERLIB_LOOP_1_OR_MORE_TIMES_PARSER_NODE_HPP


#include "parser_node.hpp"


namespace parserlib {


    template <class Parser> class loop_1_or_more_times_parser_node : public parser_node<loop_1_or_more_times_parser_node<Parser>> {
    public:
        loop_1_or_more_times_parser_node(const Parser& parser) : m_parser(parser) {
        }

        template <class ParseContext> bool parse(ParseContext& pc) const {
            if (!m_parser.parse(pc)) {
                return false;
            }
            while (m_parser.parse(pc)) {
            }
            return true;
        }

    private:
        Parser m_parser;
    };


    template <class Parser> loop_1_or_more_times_parser_node<Parser> parser_node<Parser>::operator +() const {
        return loop_1_or_more_times_parser_node<Parser>(parser(static_cast<const Parser&>(*this)));
    }


} //namespace parserlib


#endif // PARSERLIB_LOOP_1_OR_MORE_TIMES_PARSER_NODE_HPP
