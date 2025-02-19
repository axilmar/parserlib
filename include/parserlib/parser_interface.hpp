#ifndef PARSERLIB_PARSER_INTERFACE_HPP
#define PARSERLIB_PARSER_INTERFACE_HPP


#include <memory>


namespace parserlib {


    template <class ParseContext>
    class parser_interface {
    public:
        virtual ~parser_interface() {
        }

        virtual bool parse(ParseContext& context) const = 0;

        virtual bool parse_left_recursion_start(ParseContext& context) const = 0;

        virtual bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const = 0;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSER_INTERFACE_HPP
