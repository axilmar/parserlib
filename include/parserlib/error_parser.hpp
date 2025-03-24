#ifndef PARSERLIB_ERROR_PARSER_HPP
#define PARSERLIB_ERROR_PARSER_HPP


#include <type_traits>
#include <string>
#include <string_view>
#include "string_util.hpp"
#include "parser.hpp"


namespace parserlib {


    /**
     * A parser that invokes an error handler,
     * when another parser fails to parse.
     * @param Parser the parser that is tested.
     * @param ErrorHandler the error handler that is invoked when parsing fails.
     *  It's signature should be: `bool(ParseContext&)`.
     */
    template <class Parser, class ErrorHandler>
    class error_parser : public parser<error_parser<Parser, ErrorHandler>> {
    public:
        /**
         * The constructor.
         * @param parser the parser to be tested.
         * @param error_handler the handler to be invoked when the given parser fails.
         */
        error_parser(const Parser& parser, const ErrorHandler& error_handler)
            : m_parser(parser)
            , m_error_handler(error_handler)
        {
        }

        /**
         * Non-left recursion parsing.
         * If the internal parser fails to parse,
         * then the error handler is invoked.
         * @param context parse context.
         * @return true if the internal parser succeeds, otherwise whatever the error handler returns.
         */
        template <class ParseContext>
        bool parse(ParseContext& context) const {
            return m_parser.parse(context) || m_error_handler(context);
        }

        /**
         * Left recursion start parsing.
         * If the internal parser fails to parse,
         * then the error handler is invoked.
         * @param context parse context.
         * @return true if the internal parser succeeds, otherwise whatever the error handler returns.
         */
        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            return m_parser.parse_left_recursion_start(context) || m_error_handler(context);
        }

        /**
         * Left recursion continuation parsing.
         * If the internal parser fails to parse,
         * then the error handler is invoked.
         * @param context parse context.
         * @param match_start_state parse state that signifies the start of a match.
         * @return true if the internal parser succeeds, otherwise whatever the error handler returns.
         */
        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            return m_parser.parse_left_recursion_continuation(context, match_start_state) || m_error_handler(context);
        }

    private:
        const Parser m_parser;
        const ErrorHandler m_error_handler;
    };


    /**
     * A shortcut to creating an error parser.
     * @param parser the parser to be tested.
     * @param error_handler the handler to be invoked when the given parser fails.
     * @return an error parser.
     */
    template <class Parser, class ErrorHandler>
    error_parser<Parser, ErrorHandler> on_error(const Parser& parser, const ErrorHandler& error_handler) {
        return { parser, error_handler };
    }


    /**
     * It creates an error parser that invokes a function that tests the current input.
     * If the function returns true, then an error is added to the context and parsing continues
     * after the position of the error; otherwise the function is called repeatedly to each subsequent position,
     * until end of input is reached.
     * @param parser the parser to be tested.
     * @param error_id id of error to add to the context.
     * @param skip_error_token_func function to invoke at each parse position; signature: `bool(ParseContext&)`.
     * @return an error parser object that corresponds to the given parser and skip error function.
     */
    template <class Parser, class ErrorId, class SkipErrorTokenFunc>
    auto on_error_continue(const Parser& parser, const ErrorId& error_id, const SkipErrorTokenFunc& skip_error_token_func) {
        const auto error_handler = 
            [=](auto& context) {
                const auto error_start = context.parse_position();
                const auto error_pos = context.first_unparsed_position();
                while (context.is_valid_parse_position()) {
                    const auto error_end = context.parse_position();
                    if (skip_error_token_func(context)) {
                        context.add_error(error_id, error_start, error_end, error_pos);
                        return true;
                    }
                    context.increment_parse_position();
                }
                return false;
            };

        return on_error(parser, error_handler);
    }


    /**
     * It creates an error parser that recognizes the specific sequence as the end of the current scope,
     * allowing the parsing to continue after it.
     * @param parser the parser to be tested.
     * @param error_id id of error to add to the context.
     * @param skip_error_token terminal that is used for recognizing the end of a sequence; it could be any parser.
     */
    template <class Parser, class ErrorId, class SkipErrorToken>
    auto on_error_continue_after(const Parser& parser, const ErrorId& error_id, const SkipErrorToken& skip_error_token) {
        const auto skip_error_token_func =
            [terminal = get_parser_wrapper(skip_error_token)](auto& context) {
                return terminal.parse(context);
            };

        return on_error_continue(parser, error_id, skip_error_token_func);
    }


} //namespace parserlib


#endif //PARSERLIB_ERROR_PARSER_HPP
