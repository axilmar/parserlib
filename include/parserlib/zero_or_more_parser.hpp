#ifndef PARSERLIB_ZERO_OR_MORE_PARSER_HPP
#define PARSERLIB_ZERO_OR_MORE_PARSER_HPP


#include "parser.hpp"


namespace parserlib {


    template <class Parser>
    class zero_or_more_parser : public parser<zero_or_more_parser<Parser>> {
    public:
        zero_or_more_parser(const Parser& parser)
            : m_parser(parser)
        {
        }

        template <class ParseContext>
        bool parse(ParseContext& context) const noexcept {
            while (true) {
                const bool result = m_parser.parse(context);
                if (!result) {
                    break;
                }
            }
            return true;
        }

        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const noexcept {
            while (true) {
                const bool result = m_parser.parse_left_recursion_start(context);
                if (!result) {
                    break;
                }
            }
            return true;
        }

        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const noexcept {
            while (true) {
                const bool result = m_parser.parse_left_recursion_continuation(context, context.get_state());
                if (!result) {
                    break;
                }
            }
            return true;
        }

    private:
        const Parser m_parser;
    };


} //namespace parserlib


#endif //PARSERLIB_ZERO_OR_MORE_PARSER_HPP
