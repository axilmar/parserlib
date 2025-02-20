#ifndef PARSERLIB_NEWLINE_PARSER_HPP
#define PARSERLIB_NEWLINE_PARSER_HPP


#include "parser.hpp"


namespace parserlib {


    template <class Parser>
    class newline_parser : public parser<newline_parser<Parser>> {
    public:
        newline_parser(const Parser& parser) noexcept
            : m_parser(parser)
        {
        }

        template <class ParseContext>
        bool parse(ParseContext& context) const {
            if (m_parser.parse(context)) {
                context.increment_line();
                return true;
            }
            return false;
        }

        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            if (m_parser.parse_left_recursion_start(context)) {
                context.increment_line();
                return true;
            }
            return false;
        }

        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            if (m_parser.parse_left_recursion_continuation(context, context.get_state())) {
                context.increment_line();
                return true;
            }
        }

    private:
        Parser m_parser;
    };


    template <class Parser>
    newline_parser<Parser> newline(const Parser& parser) noexcept {
        return parser;
    }


} //namespace parserlib


#endif //PARSERLIB_NEWLINE_PARSER_HPP
