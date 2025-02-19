#ifndef PARSERLIB_LOGICAL_NOT_PARSER_HPP
#define PARSERLIB_LOGICAL_NOT_PARSER_HPP


#include "parser.hpp"


namespace parserlib {


    template <class Parser>
    class logical_not_parser : public parser<logical_not_parser<Parser>> {
    public:
        logical_not_parser(const Parser& parser)
            : m_parser(parser)
        {
        }

        template <class ParseContext>
        bool parse(ParseContext& context) const {
            const auto state = context.get_state();
            const bool result = !m_parser.parse(context);
            context.set_state(state);
            return result;
        }

        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            const auto state = context.get_state();
            const bool result = !m_parser.parse_left_recursion_start(context);
            context.set_state(state);
            return result;
        }

        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            const auto state = context.get_state();
            const bool result = !m_parser.parse_left_recursion_continuation(context, state);
            context.set_state(state);
            return result;
        }

    private:
        const Parser m_parser;
    };


} //namespace parserlib


#endif //PARSERLIB_LOGICAL_NOT_PARSER_HPP
