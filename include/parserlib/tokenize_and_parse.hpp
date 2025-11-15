#ifndef PARSERLIB_TOKENIZE_AND_PARSE_HPP
#define PARSERLIB_TOKENIZE_AND_PARSE_HPP


#include <algorithm>
#include "parse_context.hpp"
#include "ast.hpp"


namespace parserlib {


    template <bool SameErrorId, class TokenizerParseContext>
    struct tokenize_and_parse_result_errors {
        static constexpr bool same_error_id_type = false;
    };


    template <class TokenizerParseContext>
    struct tokenize_and_parse_result_errors<true, TokenizerParseContext> {
        static constexpr bool same_error_id_type = true;
        typename TokenizerParseContext::error_container_type errors;
    };


    template <class Source, class TokenizerParseContext, class ParserParseContext>
    struct tokenize_and_parse_result 
        : tokenize_and_parse_result_errors<std::is_same_v<typename TokenizerParseContext::error_id_type, typename ParserParseContext::error_id_type>, TokenizerParseContext>
    {
        using ast_node_type = ast_node<typename ParserParseContext::source_type, typename ParserParseContext::match_id_type, typename ParserParseContext::text_position_type>;
        using ast_node_ptr_type = std::shared_ptr<ast_node_type>;

        struct {
            TokenizerParseContext parse_context;
            bool success;
        } tokenizer;

        struct {
            ParserParseContext parse_context;
            bool success;
        } parser;

        std::vector<ast_node_ptr_type> ast;

        bool success;
    };


    template <class Source, class TokenizerParseContext, class ParserParseContext, class ASTFactory = default_ast_factory<typename ParserParseContext::source_type, typename ParserParseContext::match_id_type, typename ParserParseContext::text_position_type>>
    auto tokenize_and_parse(Source& source, rule<TokenizerParseContext>& tokenizer_grammar, rule<ParserParseContext>& parser_grammar, const ASTFactory& astNodeFactory = ASTFactory()) {
        using result_type = tokenize_and_parse_result<Source, TokenizerParseContext, ParserParseContext>;

        std::shared_ptr<result_type> result = std::make_shared<result_type>();

        //tokenize
        result->tokenizer.parse_context = TokenizerParseContext(source);
        result->tokenizer.success = tokenizer_grammar.parse(result->tokenizer.parse_context) && result->tokenizer.parse_context.errors().empty();

        //parse
        result->parser.parse_context = result->tokenizer.parse_context.template derive_parse_context<typename ParserParseContext::match_id_type, typename ParserParseContext::error_id_type>();
        result->parser.success = parser_grammar.parse(result->parser.parse_context) && result->parser.parse_context.errors().empty();

        //make ast
        result->ast = make_ast(result->parser.parse_context.matches());

        //total success
        result->success = result->tokenizer.success && result->parser.success;

        //common errors
        if constexpr (result_type::same_error_id_type) {
            //add the tokenizer errors
            for (const auto& tokenizerError : result->tokenizer.parse_context.errors()) {
                result->errors.push_back(tokenizerError);
            }

            //add the parser errors
            for (const auto& parserError : result->parser.parse_context.errors()) {
                result->errors.push_back(parse_error(parserError.id(), parserError.begin_iterator()->start_position(), parserError.end_iterator()->begin_iterator()));
            }

            //sort the errors by starting position
            std::sort(result->errors.begin(), result->errors.end(), [](const auto& errorA, const auto& errorB) {
                return errorA.begin() < errorB.begin();
            });
        }

        return result;
    }


} //namespace parserlib


#endif //PARSERLIB_TOKENIZE_AND_PARSE_HPP
