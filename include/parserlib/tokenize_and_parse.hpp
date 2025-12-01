#ifndef PARSERLIB_TOKENIZE_AND_PARSE_HPP
#define PARSERLIB_TOKENIZE_AND_PARSE_HPP


#include <algorithm>
#include "parse_context.hpp"
#include "ast.hpp"


namespace parserlib {


    template <bool SameErrorId, class LexerParseContext>
    struct tokenize_and_parse_result_errors {
        static constexpr bool same_error_id_type = false;
    };


    template <class LexerParseContext>
    struct tokenize_and_parse_result_errors<true, LexerParseContext> {
        static constexpr bool same_error_id_type = true;
        typename LexerParseContext::error_container_type errors;
    };


    /**
     * Tokenize and parse result.
     */
    template <class Source, class LexerParseContext, class ParserParseContext>
    struct tokenize_and_parse_result
        : tokenize_and_parse_result_errors<std::is_same_v<typename LexerParseContext::error_id_type, typename ParserParseContext::error_id_type>, LexerParseContext>
    {
        /** AST node type. */
        using ast_node_type = ast_node<typename ParserParseContext::source_type, typename ParserParseContext::match_id_type, typename ParserParseContext::text_position_type>;

        /** AST node pointer type. */
        using ast_node_ptr_type = std::shared_ptr<ast_node_type>;

        /** The lexer part. */
        struct {
            /** The lexer parse context. */
            LexerParseContext parse_context;

            /** The lexer parse result. */
            bool success;
        } lexer;

        /** The parser part. */
        struct {
            /** The parser parse context. */
            ParserParseContext parse_context;

            /** The parser parse result. */
            bool success;
        } parser;

        /** The created ast. */
        std::vector<ast_node_ptr_type> ast;

        /** lexer and parser success. */
        bool success;
    };


    /**
     * Function that tokenizes and parses a source.
     * @param source the source to tokenize and parse.
     * @param lexer_grammar the root rule for the lexer.
     * @param parser_grammar the root rule for the parser.
     * @param astFactory factory to use for creating AST nodes; if not given, then the default is used.
     */
    template <class Source, class LexerParseContext, class ParserParseContext, class ASTFactory = default_ast_factory<typename ParserParseContext::source_type, typename ParserParseContext::match_id_type, typename ParserParseContext::text_position_type>>
    auto tokenize_and_parse(Source& source, rule<LexerParseContext>& lexer_grammar, rule<ParserParseContext>& parser_grammar, const ASTFactory& astFactory = ASTFactory()) {
        using result_type = tokenize_and_parse_result<Source, LexerParseContext, ParserParseContext>;

        std::shared_ptr<result_type> result = std::make_shared<result_type>();

        //tokenize
        result->lexer.parse_context = LexerParseContext(source);
        result->lexer.success = result->lexer.parse_context.parse(lexer_grammar) && result->lexer.parse_context.errors().empty();

        //parse
        result->parser.parse_context = result->lexer.parse_context.template derive_parse_context<typename ParserParseContext::match_id_type, typename ParserParseContext::error_id_type>();
        result->parser.success = result->parser.parse_context.parse(parser_grammar) && result->parser.parse_context.errors().empty();

        //make ast
        result->ast = make_ast(result->parser.parse_context.matches(), astFactory);

        //total success
        result->success = result->lexer.success && result->parser.success;

        //common errors
        if constexpr (result_type::same_error_id_type) {
            //add the lexer errors
            for (const auto& lexerError : result->lexer.parse_context.errors()) {
                result->errors.push_back(lexerError);
            }

            //add the parser errors
            for (const auto& parserError : result->parser.parse_context.errors()) {
                result->errors.push_back(parse_error<Source, typename LexerParseContext::error_id_type, typename LexerParseContext::text_position_type>(parserError.id(), parserError.begin_iterator()->start_position(), parserError.end_iterator()->begin_iterator()));
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
