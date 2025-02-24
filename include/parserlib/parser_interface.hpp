#ifndef PARSERLIB_PARSER_INTERFACE_HPP
#define PARSERLIB_PARSER_INTERFACE_HPP


#include <memory>


namespace parserlib {


    /**
     * Interface for parser implementations.
     * @param ParseContext the parse context.
     */
    template <class ParseContext>
    class parser_interface {
    public:
        /**
         * Virtual destructor due to inheritance.
         */
        virtual ~parser_interface() {
        }

        /**
         * The parse function.
         * @param context the parse context.
         * @return true if parsing suceeds, false otherwise.
         */
        virtual bool parse(ParseContext& context) const = 0;

        /**
         * The parse function for the left-recursion-start state.
         * @param context the parse context.
         * @return true if parsing suceeds, false otherwise.
         */
        virtual bool parse_left_recursion_start(ParseContext& context) const = 0;

        /**
         * The parse function for the left-recursion-continuation state.
         * @param context the parse context.
         * @param match_start_state parse state that signifies the start of a match.
         * @return true if parsing suceeds, false otherwise.
         */
        virtual bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const = 0;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSER_INTERFACE_HPP
