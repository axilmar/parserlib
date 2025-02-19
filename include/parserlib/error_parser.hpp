#ifndef PARSERLIB_ERROR_PARSER_HPP
#define PARSERLIB_ERROR_PARSER_HPP


#include <type_traits>
#include <string>
#include <string_view>
#include "string_util.hpp"
#include "parser.hpp"


namespace parserlib {


    template <class Parser, class ErrorHandler>
    class error_parser : public parser<error_parser<Parser, ErrorHandler>> {
    public:
        error_parser(const Parser& parser, const ErrorHandler& error_handler)
            : m_parser(parser)
            , m_error_handler(error_handler)
        {
        }

        template <class ParseContext>
        bool parse(ParseContext& context) const {
            return m_parser.parse(context) || m_error_handler(context);
        }

        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            return m_parser.parse_left_recursion_start(context) || m_error_handler(context);
        }

        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            return m_parser.parse_left_recursion_continuation(context, match_start_state) || m_error_handler(context);
        }

    private:
        const Parser m_parser;
        const ErrorHandler m_error_handler;
    };


    template <class Parser, class ErrorHandler>
    error_parser<Parser, ErrorHandler> on_error(const Parser& parser, const ErrorHandler& error_handler) {
        return { parser, error_handler };
    }


    template <class Parser, class ErrorId, class SkipErrorTokenFunc>
    auto on_error_continue(const Parser& parser, const ErrorId& error_id, const SkipErrorTokenFunc& skip_error_token_func) {
        const auto error_handler = 
            [=](auto& context) {
                const auto error_start = context.parse_position();
                const auto error_pos = context.first_unparsed_position();
                context.set_parse_position(error_start);
                while (context.is_valid_parse_position()) {
                    if (skip_error_token_func(context)) {
                        context.add_error(error_id, error_start, context.parse_position(), error_pos);
                        return true;
                    }
                    context.increment_parse_position();
                }
                return false;
            };

        return on_error(parser, error_handler);
    }


    template <class Parser, class ErrorId, class SkipErrorToken>
    auto on_error_continue_after(const Parser& parser, const ErrorId& error_id, const SkipErrorToken& skip_error_token) {
        const auto skip_error_token_func =
            [=](auto& context) {
                if (context.compare_tokens_at_parse_position(skip_error_token) == 0) {
                        context.increment_parse_position(context.get_sequence_length(skip_error_token));
                        return true;
                    }
                    return false;
                };

        return on_error_continue(parser, error_id, skip_error_token_func);
    }


} //namespace parserlib


#endif //PARSERLIB_ERROR_PARSER_HPP
