#ifndef PARSERLIB_OPTIONAL_PARSER_HPP
#define PARSERLIB_OPTIONAL_PARSER_HPP


#include "parser.hpp"


namespace parserlib {


    template <class Parser>
    class optional_parser : public parser<optional_parser<Parser>> {
    public:
        optional_parser(const Parser& parser)
            : m_parser(parser)
        {
        }

        template <class ParseContext>
        bool parse(ParseContext& context) const {
            m_parser.parse(context);
            return true;
        }

        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            m_parser.parse_left_recursion_start(context);
            return true;
        }

        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            m_parser.parse_left_recursion_continuation(context, context.get_state());
            return true;
        }

    private:
        const Parser m_parser;
    };


} //namespace parserlib


#endif //PARSERLIB_OPTIONAL_PARSER_HPP
