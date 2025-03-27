#ifndef PARSERLIB_EXPECTED_PARSER_HPP
#define PARSERLIB_EXPECTED_PARSER_HPP


#include "parser.hpp"


namespace parserlib {


    /**
     * A parser that tests if an error has happened.
     * If so, then it adds an error to the parse context,
     * and then invokes a skip policy to allow parsing to continue.
     * @param Parser parser to use for parsing.
     * @param ErrorID the error id.
     * @param SkipPolicy the skip policy.
     */
    template <class Parser, class ErrorID, class SkipPolicy>
    class expected_parser : public parser<expected_parser<Parser, ErrorID, SkipPolicy>> {
    public:
        /**
         * The constructor.
         * @param parser the parser to use for parsing.
         * @param error_id error id to use, in case of error.
         * @param skip_policy policy to use for continuing the parsing.
         */
        expected_parser(const Parser& parser, const ErrorID& error_id, const SkipPolicy& skip_policy)
            : m_parser(parser)
            , m_error_id(error_id)
            , m_skip_policy(skip_policy)
        {
        }

        /**
         * The parse function.
         * @param context the parse context.
         * @return always true, to allow parsing to continue.
         */
        template <class ParseContext>
        bool parse(ParseContext& context) const {
            return do_parse(context, [&]() {
                return m_parser.parse(context);
            });
        }

        /**
         * The parse function for the left-recursion-start state.
         * @param context the parse context.
         * @return always true, to allow parsing to continue.
         */
        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            return do_parse(context, [&]() {
                return m_parser.parse_left_recursion_start(context);
            });
        }

        /**
         * The parse function for the left-recursion-continuation state.
         * @param context the parse context.
         * @param match_start_state parse state that signifies the start of a match.
         * @return always true, to allow parsing to continue.
         */
        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            return do_parse(context, [&]() {
                return m_parser.parse_left_recursion_continuation(context, match_start_state);
            });
        }

    private:
        Parser m_parser;
        ErrorID m_error_id;
        SkipPolicy m_skip_policy;

        template <class Context>
        void handle_error(Context& context) const {
            const auto start_position = context.first_unparsed_position();
            const auto end_position = m_skip_policy.skip(context);
            context.add_error(m_error_id, { start_position, end_position });
        }

        template <class Context, class ParseFunc>
        bool do_parse(Context& context, const ParseFunc& parse_func) const {
            const std::size_t error_count_before = context.errors().size();
            const bool ok = parse_func();
            if (ok) {
                const std::size_t error_count_after = context.errors().size();
                if (error_count_after > error_count_before) {
                    handle_error(context);
                }
            }
            else {
                handle_error(context);
            }
            return true;
        }
    };


    /**
     * A skip error policy that sets the parse context position to be just before the input that the given parser parsed successfully.
     * @param Parser parser to use for testing the input.
     */
    template <class Parser>
    class skip_before_policy {
    public:
        /**
         * Constructor.
         * @param parser parser to use for testing the input.
         */
        skip_before_policy(const Parser& parser)
            : m_parser(parser)
        {
        }

        /**
         * Increments the parse position as long as it is valid, and the test parser returns false.
         * @param context the parse context.
         * @return the context parse position before the recognized grammar.
         */
        template <class Context>
        auto skip(Context& context) const {
            while (context.is_valid_parse_position()) {
                const auto state = context.get_state();
                const bool ok = m_parser.parse(context);
                context.set_state(state);
                if (ok) {
                    break;
                }
                context.increment_parse_position();
            }
            return context.parse_position();
        }

    private:
        Parser m_parser;
    };


    /**
     * A skip error policy that sets the parse context position to be just after the input that the given parser parsed successfully.
     * @param Parser parser to use for testing the input.
     */
    template <class Parser>
    class skip_after_policy {
    public:
        /**
         * Constructor.
         * @param parser parser to use for testing the input.
         */
        skip_after_policy(const Parser& parser)
            : m_parser(parser)
        {
        }

        /**
         * Increments the parse position as long as it is valid, and the test parser returns false.
         * At exit, it sets the current parse position to be the one after the test parser parsed successfully.
         * @param context the parse context.
         * @return the context parse position before the recognized grammar.
         */
        template <class Context>
        auto skip(Context& context) const {
            while (context.is_valid_parse_position()) {
                const auto state = context.get_state();
                const bool ok = m_parser.parse(context);
                if (ok) {
                    const auto new_current_parse_position = context.parse_position();
                    context.set_state(state);
                    context.set_parse_position(new_current_parse_position);
                    return state.parse_position();
                }
                context.set_state(state);
                context.increment_parse_position();
            }
            return context.parse_position();
        }

    private:
        Parser m_parser;
    };


    /**
     * A skip count policy that skips a specific number of input tokens.
     */
    class skip_count_policy {
    public:
        /**
         * The constructor.
         * @param count number of tokens to skip.
         */
        skip_count_policy(std::size_t count)
            : m_count(count)
        {
        }

        /**
         * Skips the given number of tokens, until the end of input.
         * @param context the parse context.
         * @return the context parse position after the skip loop.
         */
        template <class Context>
        auto skip(Context& context) const {
            for (std::size_t counter = 0; context.is_valid_parse_position() && counter < m_count; ++counter) {
                context.increment_parse_position();
            }
            return context.parse_position();
        }

    private:
        std::size_t m_count;
    };


    /**
     * A skip policy that skips nothing.
     * Used in cases the parse position should not be incremented on error.
     */
    class skip_nothing_policy {
    public:
        /**
         * Does nothing.
         * @param context the parse context.
         * @return the context's parse position.
         */
        template <class Context>
        auto skip(Context& context) const {
            return context.parse_position();
        }
    };


    /**
     * Creates a skip before policy for the given parser.
     * @param parser parser; it can also be a token.
     * @return a skip before policy.
     */
    template <class Parser>
    auto skip_before(const Parser& parser) {
        return skip_before_policy(get_parser_wrapper(parser));
    }


    /**
     * Creates a skip after policy for the given parser.
     * @param parser parser; it can also be a token.
     * @return a skip after policy.
     */
    template <class Parser>
    auto skip_after(const Parser& parser) {
        return skip_after_policy(get_parser_wrapper(parser));
    }


    /**
     * Creates a skip count policy.
     * @param count number of tokens to skip.
     * @return a skip count policy.
     */
    inline skip_count_policy skip_count(std::size_t count) {
        return count;
    }


    /**
     * Creates a skip count policy for skipping 1 token.
     * @return a skip count policy with count = 1.
     */
    inline skip_count_policy skip_one() {
        return skip_count(1);
    }


    /**
     * Creates an expected parser out of the given parser, error id and skip policy.
     * @param parser the parser to use for parsing; it can also be a token.
     * @param error_id the error id of the error to put into the parse context.
     * @param skip_policy the skip policy.
     * @return the appropriate expected parser.
     */
    template <class Parser, class ErrorID, class SkipPolicy>
    auto expected(const Parser& parser, const ErrorID& error_id, const SkipPolicy& skip_policy) {
        return expected_parser(get_parser_wrapper(parser), error_id, skip_policy);
    }


    /**
     * Creates an expected parser out of the given parser and error id.
     * The skip policy is 'skip_nothing_policy'.
     * @param parser the parser to use for parsing; it can also be a token.
     * @param error_id the error id of the error to put into the parse context.
     * @return the appropriate expected parser.
     */
    template <class Parser, class ErrorID>
    auto expected(const Parser& parser, const ErrorID& error_id) {
        return expected_parser(get_parser_wrapper(parser), error_id, skip_nothing_policy());
    }


} //namespace parserlib


#endif //PARSERLIB_EXPECTED_PARSER_HPP
