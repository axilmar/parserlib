#ifndef PARSERLIB_COMPILER_FRONTEND_FUNCTIONS_HPP
#define PARSERLIB_COMPILER_FRONTEND_FUNCTIONS_HPP


#include <tuple>
#include <vector>
#include "parse_context.hpp"
#include "token.hpp"
#include "error.hpp"
#include "ast_node.hpp"


namespace parserlib {


    template <class Grammar>
    class compiler_frontend {
    public:
        using grammar_type = Grammar;

        using lexer_type = typename grammar_type::lexer;

        using parser_type = typename grammar_type::parser;

        class lexer {
        public:
            using token_id_type = typename lexer_type::token_id_type;

            using character_comparator_type = typename lexer_type::character_comparator_type;

            using error_id_type = typename lexer_type::error_id_type;

            template <class Input>
            using parse_definitions_type = parse_definitions<Input, token_id_type, error_id_type, character_comparator_type>;

            template <class Input>
            struct result {
                using token_type = token<Input, token_id_type>;

                using token_container_type = std::vector<token_type>;

                using error_type = error<parse_definitions_type<Input>>;

                using error_container_type = std::vector<error_type>;

                bool success;

                token_container_type tokens;

                error_container_type errors;
            };

            template <class Input>
            static result<Input> tokenize(Input& input) {
                //types
                using parse_definitions_type = parse_definitions_type<Input>;
                using result_type = result<Input>;
                using token_container_type = typename result_type::token_container_type;
                using parse_context_type = parse_context<parse_definitions_type>;
                using grammar_type = typename lexer_type::template grammar<parse_context_type>;

                //parse
                parse_context_type context(input);
                const bool success = grammar_type().parse(context);

                //gather tokens
                token_container_type tokens;
                for (const auto& match : context.matches()) {
                    tokens.emplace_back(match.id(), match.span());
                }

                //return result
                return { success && context.errors().empty(), tokens, context.errors()};
            }

        }; //class lexer

        class parser {
        public:
            using ast_id_type = typename parser_type::ast_id_type;

            using error_id_type = typename parser_type::error_id_type;

            template <class Input>
            using parse_definitions_type = parse_definitions<Input, ast_id_type, error_id_type>;

            template <class Input>
            struct result {
                using parse_definitions_type = parse_definitions_type<Input>;

                using ast_node_ptr_type = parserlib::ast_node_ptr_type<parse_definitions_type>;

                using ast_container_type = std::vector<ast_node_ptr_type>;

                using error_type = error<parse_definitions_type>;

                using error_container_type = std::vector<error_type>;

                bool success;

                ast_container_type ast_nodes;

                error_container_type errors;
            };

            template <class Input, class AstNodeFactory>
            static result<Input> parse(Input& input, const AstNodeFactory& ast_node_factory) {
                //types
                using parse_definitions_type = parse_definitions_type<Input>;
                using result_type = result<Input>;
                using ast_container_type = typename result_type::ast_container_type;
                using parse_context_type = parse_context<parse_definitions_type>;
                using grammar_type = typename parser_type::template grammar<parse_context_type>;

                //parse
                parse_context_type context(input);
                const bool success = grammar_type().parse(context);

                //create the ast node tree
                const auto ast_nodes = create_ast_nodes(context.matches(), ast_node_factory);

                //return result
                return { success && context.errors().empty(), ast_nodes, context.errors()};
            }

            template <class Input>
            static result<Input> parse(Input& input) {
                return parse(input, ast_node_factory<ast_node<parse_definitions_type<Input>>>());
            }
        };

        template <class Input>
        struct result {
            using tokenizer_input_type = std::vector<token<Input, typename lexer_type::token_id_type>>;

            bool success;

            typename lexer::template result<Input> lexer;

            typename parser::template result<tokenizer_input_type> parser;

        };
    
        template <class Input> 
        static result<Input> parse(Input& input) {
            result<Input> result;
            result.lexer = lexer::tokenize(input);
            result.parser = parser::parse(result.lexer.tokens);
            result.success = result.lexer.success && result.parser.success;
            return result;
        }
    };


} //namespace parserlib


#endif //PARSERLIB_COMPILER_FRONTEND_FUNCTIONS_HPP
