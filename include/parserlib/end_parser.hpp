#ifndef PARSERLIB_END_PARSER_HPP
#define PARSERLIB_END_PARSER_HPP


#include "parser.hpp"


namespace parserlib {


    /**
     * A parser that signifies the end of parsing.
     * It is used in cases where the whole input must be successfully parsed
     * in order to consider a parse successful.
     */
    class end_parser : public parser<end_parser> {
    public:
        /**
         * Parses for end of input.
         * @param context parse context.
         * @return true if the end is reached, false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& context) const {
            return context.is_end_parse_position();
        }

        /**
         * Parses for end of input.
         * @param context parse context.
         * @return true if the end is reached, false otherwise.
         */
        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            return parse(context);
        }

        /**
         * Parses for end of input.
         * @param context parse context.
         * @param match_start_state parse state that signifies the start of a match.
         * @return true if the end is reached, false otherwise.
         */
        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            return parse(context);
        }
    };


    inline end_parser end() {
        return {};
    }


} //namespace parserlib


#endif //PARSERLIB_END_PARSER_HPP
