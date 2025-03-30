#ifndef PARSERLIB_PARSER_HPP
#define PARSERLIB_PARSER_HPP


#include <vector>
#include "parse_context.hpp"
#include "ast.hpp"


namespace parserlib {


    template <class TokenId, class Iterator>
    class parsed_token {
    public:
        using token_type = parsed_token<TokenId, Iterator>;
        using token_id_type = TokenId;
        using iterator_type = Iterator;

        parsed_token(const token_id_type& token_id, const iterator_type& begin, const iterator_type& end) noexcept
            : m_id(token_id)
            , m_begin(begin)
            , m_end(end)
        {
        }

        const token_id_type& id() const noexcept {
            return m_id;
        }

        const iterator_type& begin() const noexcept {
            return m_begin;
        }

        const iterator_type& end() const noexcept {
            return m_end;
        }

        auto source() const noexcept {
            return parserlib::source(m_begin, m_end);
        }

        template <class MatchId>
        friend int operator - (const token_type& token, const MatchId& match_id) noexcept {
            return static_cast<int>(token.m_id) - static_cast<int>(match_id);
        }

        template <class MatchId>
        friend int operator - (const MatchId& match_id, const token_type& token) noexcept {
            return static_cast<int>(match_id) - static_cast<int>(token.m_id);
        }

    private:
        token_id_type m_id;
        iterator_type m_begin;
        iterator_type m_end;
    };


    template <class Grammar, class Source>
    class lexer {
    public:
        using grammar_type = Grammar;
        using match_id_type = typename grammar_type::match_id_type;
        using error_id_type = typename grammar_type::error_id_type;
        using comparator_type = typename grammar_type::comparator_type;

        using source_type = Source;
        using iterator_type = typename source_type::const_iterator;

        using parsed_token_type = parsed_token<match_id_type, iterator_type>;
        using parsed_token_container_type = std::vector<parsed_token_type>;

        using error_type = parse_error<error_id_type, iterator_type>;
        using error_container_type = std::vector<error_type>;

        struct result {
            bool success;
            parsed_token_container_type parsed_tokens;
            error_container_type errors;
        };

        static result parse(Source& source) noexcept {
            //parse
            using parse_context_type = parse_context<source_type, match_id_type, error_id_type, comparator_type>;
            parse_context_type pc(source);
            Grammar grammar;
            const bool success = grammar.parse(pc) && pc.is_end_parse_position();

            //prepare the parsed tokens
            parsed_token_container_type parsed_tokens;
            for (const auto& m : pc.matches()) {
                parsed_tokens.push_back(parsed_token_type(m.id(), m.begin(), m.end()));
            }

            //if the whole input is not consumed, then add an error to the parsed position
            if (!pc.is_end_parse_position()) {
                pc.add_error(Grammar::incomplete_parse_error_id(), pc.parse_position(), source.end());
            }

            //sort the errors
            pc.sort_errors();

            //return the result
            return { success, parsed_tokens, pc.errors() };
        }
    };


    template <class LexerGrammar, class ParserGrammar, class Source>
    class parser {
    public:
        using lexer_type = lexer<LexerGrammar, Source>;
        using iterator_type = typename lexer_type::iterator_type;

        using parser_grammar_type = ParserGrammar;
        using node_id_type = typename parser_grammar_type::match_id_type;

        using ast_node_type = ast_node<node_id_type, iterator_type>;
        using ast_node_ptr_type = parserlib::ast_node_ptr_type<node_id_type, iterator_type>;
        using ast_node_container_type = parserlib::ast_node_container_type<node_id_type, iterator_type>;

        using error_id_type = typename parser_grammar_type::error_id_type;
        using error_type = parse_error<error_id_type, iterator_type>;
        using error_container_type = std::vector<error_type>;

        struct result {
            bool success;
            ast_node_container_type ast_nodes;
            error_container_type errors;
        };

        template <class AstFactory>
        static result parse(Source& source, const AstFactory& ast_factory) noexcept {
            //tokenize
            typename lexer_type::result lexer_result = lexer_type::parse(source);

            //parse the tokens
            ParserGrammar grammar;
            parse_context<typename lexer_type::parsed_token_container_type, node_id_type, error_id_type, case_sensitive_comparator> pc(lexer_result.parsed_tokens);
            const bool success = grammar.parse(pc) && pc.is_end_parse_position();

            //create the ast nodes
            ast_node_container_type ast_nodes;
            for (const auto& m : pc.matches()) {
                ast_nodes.push_back(create_ast_node(m, ast_factory));
            }

            error_container_type errors;

            //translate the lexer errors to parser errors
            for (const auto& lexer_error : lexer_result.errors()) {
                errors.push_back(error_type(ParserGrammar::translate_lexer_error_id(lexer_error.id()), lexer_error.begin(), lexer_error.end()));
            }

            //if the whole input is not consumed, then add an error to the parsed position
            if (!pc.is_end_parse_position()) {
                pc.add_error(ParserGrammar::incomplete_parse_error_id(), pc.parse_position(), lexer_result.parsed_tokens.end());
            }

            //translate the errors to the result errors; the result errors shall contain source positions, not token positions
            for (const auto& error : pc.errors()) {
                errors.push_back(error_type(error.id(), error.begin()->begin(), error.end()->begin()));
            }

            //sort the errors
            pc.sort_errors();

            //return the result
            return { success, ast_nodes, errors };
        }

        class default_ast_factory {
        public:
            ast_node_ptr_type operator ()(const node_id_type& node_id, const iterator_type& begin, const iterator_type& end, ast_node_container_type&& children) const noexcept {
                return std::make_shared<ast_node_type>(node_id, begin, end, std::move(children));
            }
        };

        static result parse(Source& source) noexcept {
            return parse(source, default_ast_factory());
        }

    private:
        template <class Match, class AstFactory>
        static ast_node_ptr_type create_ast_node(const Match& match, const AstFactory& ast_factory) noexcept {
            ast_node_container_type children;
            for (const auto& child_match : match.children()) {
                children.push_back(create_ast_node(child_match, ast_factory));
            }
            return ast_factory(match.id(), match.begin()->begin(), match.end()->begin(), std::move(children));
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSER_HPP
