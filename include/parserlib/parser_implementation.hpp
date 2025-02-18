#ifndef PARSERLIB_PARSER_IMPLEMENTATION_HPP
#define PARSERLIB_PARSER_IMPLEMENTATION_HPP


#include "parser_interface.hpp"


namespace parserlib {


    template <class ParseContext, class Parser>
    class parser_implementation : public parser_interface<ParseContext> {
    public:
        parser_implementation(const Parser& parser)
            : m_parser(parser)
        {
        }

        bool parse(ParseContext& context) const noexcept override {
            return m_parser.parse(context);
        }

        bool parse_left_recursion_start(ParseContext& context) const noexcept override {
            return m_parser.parse_left_recursion_start(context);
        }

        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const noexcept override {
            return m_parser.parse_left_recursion_continuation(context, match_start_state);
        }

        std::unique_ptr<parser_interface<ParseContext>> clone() const noexcept override {
            return std::make_unique<parser_implementation<ParseContext, Parser>>(m_parser);
        }

    private:
        const Parser m_parser;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSER_IMPLEMENTATION_HPP
