#ifndef PARSERLIB_COMPILER_FRONTEND_FUNCTIONS_HPP
#define PARSERLIB_COMPILER_FRONTEND_FUNCTIONS_HPP


#include <tuple>
#include <vector>
#include "parse_context.hpp"
#include "token.hpp"
#include "error.hpp"


namespace parserlib {


    template <class Grammar>
    class compiler_frontend {
    public:
        using grammar_type = Grammar;

        using lexer_type = typename grammar_type::lexer;

        class lexer {
        public:
            using token_id_type = typename lexer_type::token_id_type;

            using character_comparator_type = typename lexer_type::character_comparator_type;

            using error_id_type = typename lexer_type::error_id_type;

            template <class Input>
            using parse_definitions_type = parse_definitions<Input, token_id_type, error_id_type, character_comparator_type>;

            template <class Input>
            struct tokenizer_result {
                using token_type = token<Input, token_id_type>;

                using token_container_type = std::vector<token_type>;

                using error_type = error<parse_definitions_type<Input>>;

                using error_container_type = std::vector<error_type>;

                bool success;

                token_container_type tokens;

                error_container_type errors;
            };

            template <class Input>
            static tokenizer_result<Input> tokenize(Input& input) {
                //types
                using parse_definitions_type = parse_definitions_type<Input>;
                using result_type = tokenizer_result<Input>;
                using token_container_type = typename result_type::token_container_type;

                //parse
                parse_context<parse_definitions_type> context(input);
                const bool success = lexer_type::grammar().parse(context);

                //gather tokens
                token_container_type tokens;
                for (const auto& match : context.matches()) {
                    tokens.emplace_back(match.id(), match.span());
                }

                //return result
                return { success, tokens, context.errors() };
            }

        }; //class lexer
    };


} //namespace parserlib


#endif //PARSERLIB_COMPILER_FRONTEND_FUNCTIONS_HPP
