#ifndef PARSERLIB_END_PARSER_HPP
#define PARSERLIB_END_PARSER_HPP


#include "parser.hpp"


namespace parserlib {


    class end_parser : public parser<end_parser> {
    public:
        template <class ParseContext>
        bool parse(ParseContext& context) const noexcept {
            return context.is_end_parse_position();
        }

        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const noexcept {
            return parse(context);
        }

        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const noexcept {
            return parse(context);
        }
    };


    inline end_parser end() {
        return {};
    }


} //namespace parserlib


#endif //PARSERLIB_END_PARSER_HPP
