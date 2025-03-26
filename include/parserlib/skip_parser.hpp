#ifndef PARSERLIB_SKIP_PARSER_HPP
#define PARSERLIB_SKIP_PARSER_HPP


#include "parser.hpp"


namespace parserlib {


    /**
     * A parser that skips input until a specific parser parses successfully.
     * The context is setup to continue parsing before the input recognized by the parser.
     * @param Parser type of parser to use for testing the input.
     * @param ErrorID error id type.
     */
    template <class Parser, class ErrorID>
    class skip_to_parser : public parser<skip_to_parser<Parser, ErrorID>> {
    public:
        /**
         * The constructor.
         * @param parser the internal parser to use for testing the input.
         * @param error error id.
         */
        skip_to_parser(const Parser& parser, const ErrorID& error_id)
            : m_parser(parser)
            , m_error_id(error_id)
        {
        }

        /**
         * Non-left recursion parsing.
         * Skips the current input until the internal parser parses successfully.
         * @param context parse context.
         * @return true if parsing succeeded, false if the end of input is reached.
         */
        template <class ParseContext>
        bool parse(ParseContext& context) const {
            return do_parse(context, [&]() { return m_parser.parse(context); });
        }

        /**
         * Left recursion start parsing.
         * Skips the current input until the internal parser parses successfully.
         * @param context parse context.
         * @return true if parsing succeeded, false if the end of input is reached.
         */
        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            return do_parse(context, [&]() { return m_parser.parse_left_recursion_start(context); });
        }

        /**
         * Left recursion continuation parsing.
         * Skips the current input until the internal parser parses successfully.
         * @param context parse context.
         * @param match_start_state parse state that signifies the start of a match.
         * @return true if parsing succeeded, false if the end of input is reached.
         */
        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            return do_parse(context, [&]() { return m_parser.parse_left_recursion_continuation(context, match_start_state); });
        }

    private:
        Parser m_parser;
        ErrorID m_error_id;

        //do a parse, calling the given function to parse
        template <class Context, class ParseFunc>
        bool do_parse(Context& context, const ParseFunc& parse_func) const {
            //the error span starts from the current parse position
            const auto error_start = context.parse_position();

            //the error starts from the first unparsed position
            const auto error_pos = context.first_unparsed_position();

            //continue skipping the error from the error position
            context.set_parse_position(error_pos);

            //loop while the input is available
            while (context.is_valid_parse_position()) {

                //parse; make sure the context state is not affected
                const auto state = context.get_state();
                const bool ok = parse_func();
                context.set_state(state);

                //if parsed successfully, then add an error and return true
                if (ok) {
                    context.add_error(m_error_id, error_start, context.parse_position(), error_pos);
                    return true;
                }

                //failed to parse; try the next position
                context.increment_parse_position();
            }

            //failure; the skip grammar was not recognized
            return false;

        }
    };


    /**
     * A parser that skips input until a specific parser parses successfully.
     * The context is setup to continue parsing after the input recognized by the parser.
     * @param Parser type of parser to use for testing the input.
     * @param ErrorID error id type.
     */
    template <class Parser, class ErrorID>
    class skip_after_parser : public parser<skip_after_parser<Parser, ErrorID>> {
    public:
        /**
         * The constructor.
         * @param parser the internal parser to use for testing the input.
         * @param error error id.
         */
        skip_after_parser(const Parser& parser, const ErrorID& error_id)
            : m_parser(parser)
            , m_error_id(error_id)
        {
        }

        /**
         * Non-left recursion parsing.
         * Skips the current input until the internal parser parses successfully.
         * @param context parse context.
         * @return true if parsing succeeded, false if the end of input is reached.
         */
        template <class ParseContext>
        bool parse(ParseContext& context) const {
            return do_parse(context, [&]() { return m_parser.parse(context); });
        }

        /**
         * Left recursion start parsing.
         * Skips the current input until the internal parser parses successfully.
         * @param context parse context.
         * @return true if parsing succeeded, false if the end of input is reached.
         */
        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            return do_parse(context, [&]() { return m_parser.parse_left_recursion_start(context); });
        }

        /**
         * Left recursion continuation parsing.
         * Skips the current input until the internal parser parses successfully.
         * @param context parse context.
         * @param match_start_state parse state that signifies the start of a match.
         * @return true if parsing succeeded, false if the end of input is reached.
         */
        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            return do_parse(context, [&]() { return m_parser.parse_left_recursion_continuation(context, match_start_state); });
        }

    private:
        Parser m_parser;
        ErrorID m_error_id;

        //do a parse, calling the given function to parse
        template <class Context, class ParseFunc>
        bool do_parse(Context& context, const ParseFunc& parse_func) const {
            //the error span starts from the current parse position
            const auto error_start = context.parse_position();

            //the error starts from the first unparsed position
            const auto error_pos = context.first_unparsed_position();

            //continue skipping the error from the error position
            context.set_parse_position(error_pos);

            //loop while the input is available
            while (context.is_valid_parse_position()) {

                //parse; make sure the context state is not affected
                const auto state = context.get_state();
                const bool ok = parse_func();
                const auto next_parse_position = context.parse_position();
                context.set_state(state);

                //if parsed successfully, then add an error and return true
                if (ok) {
                    context.add_error(m_error_id, error_start, context.parse_position(), error_pos);
                    context.set_parse_position(next_parse_position);
                    return true;
                }

                //failed to parse; try the next position
                context.increment_parse_position();
            }

            //failure; the skip grammar was not recognized
            return false;

        }
    };


    /**
     * Creates a skip parser that parses input until the given grammar is recognized.
     * @param parser the internal parser to use for testing the input.
     * @param error error id.
     * @return a skip parser.
     */
    template <class Parser, class ErrorID>
    auto skip_to(const Parser& parser, const ErrorID& error_id) {
        return skip_to_parser(get_parser_wrapper(parser), error_id);
    }


    /**
     * Creates a skip parser that parses input until after the given grammar is recognized.
     * @param parser the internal parser to use for testing the input.
     * @param error error id.
     * @return a skip parser.
     */
    template <class Parser, class ErrorID>
    auto skip_after(const Parser& parser, const ErrorID& error_id) {
        return skip_after_parser(get_parser_wrapper(parser), error_id);
    }


} //namespace parserlib


#endif //PARSERLIB_SKIP_PARSER_HPP
