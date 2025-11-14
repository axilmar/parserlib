#ifndef PARSERLIB_TOKENIZE_AND_PARSE_HPP
#define PARSERLIB_TOKENIZE_AND_PARSE_HPP


#include "parse_context.hpp"
#include "ast.hpp"


namespace parserlib {


    template <class Source, class TokenizerParseContext, class ParserParseContext>
    struct tokenize_and_parse_result {
        struct {
            TokenizerParseContext parse_context;
            bool success;
        } tokenizer;

        struct {
            ParserParseContext parse_context;
            bool success;
        } parser;

        using ast_node_type = ast_node<typename ParserParseContext::source_type, typename ParserParseContext::match_id_type, typename ParserParseContext::text_position_type>;

        using ast_node_ptr_type = std::shared_ptr<ast_node_type>;

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

        return result;
    }


} //namespace parserlib


#endif //PARSERLIB_TOKENIZE_AND_PARSE_HPP
