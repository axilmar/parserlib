#ifndef PARSERLIB_EBNF_HPP
#define PARSERLIB_EBNF_HPP


#include "parserlib.hpp"


namespace parserlib {


    class ebnf {
    public:
        enum class token_id {
        };

        template <class Source>
        class tokenizer {
        public:
            using tokenizer_parse_context = parserlib::parse_context<Source, token_id, error_id, text_position>();
            using rule_type = rule<tokenizer_parse_context>;

            class grammar : public rule_type {
            public:
                using rule_type::operator =;

                grammar() {
                }
            };
        };

        template <class Source>
        class parser {
        public:
        };
    };


} //namespace parserlib


#endif //PARSERLIB_EBNF_HPP
