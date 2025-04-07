#ifndef PARSERLIB_PARSER_HPP
#define PARSERLIB_PARSER_HPP


#include <vector>
#include "parse_context.hpp"
#include "ast.hpp"


namespace parserlib {


    /**
     * A parsed token.
     * @param TokenId id of token.
     * @param Iterator iterator into the source.
     */
    template <class TokenId, class Iterator>
    class parsed_token {
    public:
        /** This class type. */
        using token_type = parsed_token<TokenId, Iterator>;

        /** Token id type. */
        using token_id_type = TokenId;

        /** Iterator type. */
        using iterator_type = Iterator;

        /**
         * The constructor.
         * @param token_id id of the token.
         * @param begin begin iterator into the source.
         * @param end end iterator into the source.
         */
        parsed_token(const token_id_type& token_id, const iterator_type& begin, const iterator_type& end) noexcept
            : m_id(token_id)
            , m_begin(begin)
            , m_end(end)
        {
            assert(begin < end);
        }

        /**
         * Returns the id of the token.
         * @return the id of the token.
         */
        const token_id_type& id() const noexcept {
            return m_id;
        }

        /**
         * Returns an iterator to the beginning of the part of the source that this token represents.
         * @return an iterator to the beginning of the part of the source that this token represents.
         */
        const iterator_type& begin() const noexcept {
            return m_begin;
        }

        /**
         * Returns an iterator to the end of the part of the source that this token represents.
         * @return an iterator to the end of the part of the source that this token represents.
         */
        const iterator_type& end() const noexcept {
            return m_end;
        }

        /**
         * Returns the part of the source that this token represents.
         * @return either an std string view of the source, if the element satisfies the string view requirements,
         *  or an std vector.
         */
        auto source() const noexcept {
            return parserlib::source(m_begin, m_end);
        }

        /**
         * Computes the numeric difference between the given token and match id.
         * Required so as that parsed tokens are comparable to match ids.
         * @param token token.
         * @param match_id match id.
         * @return the numeric difference between the token's id and the given match id.
         */
        template <class MatchId>
        friend int operator - (const token_type& token, const MatchId& match_id) noexcept {
            return static_cast<int>(token.m_id) - static_cast<int>(match_id);
        }

        /**
         * Computes the numeric difference between the given token and match id.
         * Required so as that parsed tokens are comparable to match ids.
         * @param match_id match id.
         * @param token token.
         * @return the numeric difference between the token's id and the given match id.
         */
        template <class MatchId>
        friend int operator - (const MatchId& match_id, const token_type& token) noexcept {
            return static_cast<int>(match_id) - static_cast<int>(token.m_id);
        }

    private:
        token_id_type m_id;
        iterator_type m_begin;
        iterator_type m_end;
    };


    /**
     * An empty enumeration representing an empty error id type.
     * Provided automatically when a grammar does not define an 'error_id_type'.
     */
    enum empty_error_id_type {
    };


    /**
     * Trait class to discover if a grammar contains an error id type.
     * If not provided, then the enum 'empty_error_id_type' is provided.
     * @param T type of grammar to test.
     */
    template <class T>
    class get_grammar_error_id_type {
    private:
        template <class U> static empty_error_id_type test(...);
        template <class U> static typename U::error_id_type test(typename U::error_id_type*);

    public:
        /**
         * The error id type of the grammar; if not provided, then 'empty_error_id_type'.
         */
        using type = decltype(test<T>(0));
    };


    /**
     * Trait class to discover if a grammar contains a comparator type.
     * If not provided, then the comparator 'case_sensitive_comparator' is used.
     * @param T type of grammar to test.
     */
    template <class T>
    class get_grammar_comparator_type {
    private:
        template <class U> static case_sensitive_comparator test(...);
        template <class U> static typename U::comparator_type test(typename U::comparator_type*);

    public:
        /**
         * The comparator type of a grammar.
         * If the grammar does not contain a 'comparator_type', then the 'case_sensitive_comparator' is used.
         */
        using type = decltype(test<T>(0));
    };


    /**
     * Trait class to discover if a grammar contains a function named 'incomplete_parse_error_id()'.
     * This function is used to discover if an error 'incomplete parse' shall be added at the end
     * of a parse, if the whole input is not consumed.
     * @param T type of grammar to test.
     */
    template <class T>
    class grammar_has_incomplete_parse_error_id {
    private:
        struct no { char c[1]; };
        struct yes { char c[2]; };
        template <class U> static no test(...);
        template <class U> static yes test(decltype(&U::incomplete_parse_error_id)*);

    public:
        /**
         * True if the grammar contains the function 'incomplete_parse_error_id()',
         * false otherwise.
         */
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes);
    };


    /**
     * Trait class to discover if a parser grammar contains a function named 'translate_lexer_error_id(lexer_error_id)'.
     * This function is used to translate lexer errors to parser errors, so as that at the end of a parse,
     * the error list also contains the lexer errors.
     * This can be avoided if a lexer and a parser share the same error id enumeration.
     * @param T type of grammar to test.
     */
    template <class T>
    class parser_grammar_has_translate_lexer_error_id {
    private:
        struct no { char c[1]; };
        struct yes { char c[2]; };
        template <class U> static no test(...);
        template <class U> static yes test(decltype(U::translate_lexer_error_id)*);

    public:
        /**
         * True if the grammar contains a function 'translate_lexer_error_id(lexer_error_id)',
         * false otherwise.
         */
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes);
    };


    /**
     * A class that provides the typedefs and functions required for tokenization.
     *
     * @param Grammar the grammar to use for tokenization.
     *
     *  The grammar must have the following members:
     *      - type match_id_type: enumeration of the match id.
     *      - template <class ParseContext> parse_result parse(ParseContext& pc): function to use for parsing.
     *
     *  The grammar can have the following optional members:
     *      - error_id_type: enumeration of the error id.
     *      - comparator_type: character comparator (see the class parse_context for details).
     *
     * @param Source the type of source; it must define a const_iterator and value_type.
     */
    template <class Grammar, class Source>
    class lexer {
    public:
        /** Grammar type. */
        using grammar_type = Grammar;

        /** Match id type. */
        using match_id_type = typename grammar_type::match_id_type;

        /** Error id type. */
        using error_id_type = typename get_grammar_error_id_type<Grammar>::type;

        /** Comparator type. */
        using comparator_type = typename get_grammar_comparator_type<Grammar>::type;

        /** Source type. */
        using source_type = Source;

        /** Iterator type. */
        using iterator_type = typename source_type::const_iterator;

        /** Parsed token type. */
        using parsed_token_type = parsed_token<match_id_type, iterator_type>;

        /** Parsed token container type. */
        using parsed_token_container_type = std::vector<parsed_token_type>;

        /** Error type. */
        using error_type = parse_error<error_id_type, iterator_type>;

        /** Error container type. */
        using error_container_type = std::vector<error_type>;

        /** Parse result type. */
        struct result {
            /** Parsing success flag. */
            bool success;

            /** Parsed tokens. */
            parsed_token_container_type parsed_tokens;

            /** Errors. */
            error_container_type errors;
        };

        /**
         * The parse function.
         * @param source the source to parse.
         * @param extension extension to use.
         * @return the parse result.
         */
        template <class Extension = empty_parse_context_extension>
        static result parse(Source& source, const Extension& extension = Extension()) noexcept {
            //parse
            using parse_context_type = parse_context<source_type, match_id_type, error_id_type, comparator_type, Extension>;
            parse_context_type pc(source, extension);
            Grammar grammar;
            const bool success = grammar.parse(pc) && pc.is_end_parse_position();

            //prepare the parsed tokens
            parsed_token_container_type parsed_tokens;
            for (const auto& m : pc.matches()) {
                parsed_tokens.push_back(parsed_token_type(m.id(), m.begin(), m.end()));
            }

            //if the whole input is not consumed, then add an error to the parsed position
            if constexpr (grammar_has_incomplete_parse_error_id<Grammar>::value) {
                if (!pc.is_end_parse_position()) {
                    pc.add_error(Grammar::incomplete_parse_error_id, pc.parse_position(), source.end());
                }
            }

            //sort the errors
            pc.sort_errors();

            //return the result
            return { success && pc.errors().empty(), parsed_tokens, pc.errors() };
        }
    };


    /**
     * Base class for AST factory classes.
     * Used for recognizing when a type is an AST factory.
     */
    class ast_factory_base {
    };


    /**
     * A type that provides a lexer and a parser.
     * It first tokenizes the input, then parses it according to a grammar to create an AST tree.
     *
     * @param LexerGrammar lexer grammar type; see class lexer for details.
     *
     * @param ParserGrammar parser grammar type.
     *
     *  The parser grammar must have the following members:
     *      - type match_id_type: enumeration of the match id.
     *
     *  The grammar can have the following optional members:
     *      - error_id_type: enumeration of the error id.
     *
     * @param Source the type of source; it must define a const_iterator and value_type.
     */
    template <class LexerGrammar, class ParserGrammar, class Source>
    class parser {
    public:
        /** The lexer type. */
        using lexer_type = lexer<LexerGrammar, Source>;

        /** The source iterator type. */
        using iterator_type = typename lexer_type::iterator_type;

        /** The parser grammar type. */
        using parser_grammar_type = ParserGrammar;

        /** The node id type is the match id type of the parser grammar. */
        using node_id_type = typename parser_grammar_type::match_id_type;

        /** The AST node type. */
        using ast_node_type = ast_node<node_id_type, iterator_type>;

        /** The AST node ptr type. */
        using ast_node_ptr_type = parserlib::ast_node_ptr_type<node_id_type, iterator_type>;

        /** The AST node container type. */
        using ast_node_container_type = parserlib::ast_node_container_type<node_id_type, iterator_type>;

        /** The error id type. */
        using error_id_type = typename get_grammar_error_id_type<ParserGrammar>::type;

        /** The error type. */
        using error_type = parse_error<error_id_type, iterator_type>;

        /** The error container type. */
        using error_container_type = std::vector<error_type>;

        /** The parse result. */
        struct result {
            /** Parsing success flag. */
            bool success;

            /** The AST nodes. */
            ast_node_container_type ast_nodes;

            /** Errors. */
            error_container_type errors;
        };

        /**
         * The default AST factory creates instances of the default AST node type.
         */
        class default_ast_factory : public ast_factory_base {
        public:
            /**
             * Creates an instance of the class ast_node.
             * @param node_id id of the node.
             * @param begin iterator to the beginning of the part of the source the AST node represents.
             * @param end iterator to the end of the part of the source the AST node represents.
             * @param children children nodes.
             */
            ast_node_ptr_type operator ()(const node_id_type& node_id, const iterator_type& begin, const iterator_type& end, ast_node_container_type&& children) const noexcept {
                return std::make_shared<ast_node_type>(node_id, begin, end, std::move(children));
            }
        };

        /**
         * Given a source, it first tokenizes the source, then parses the tokens using the parser grammar.
         * @param source the source to parse.
         * @param ast_factory the class that creates the AST nodes; it can be overriden so as that custom AST nodes are created.
         * @param extension the extension to use for the parse context.
         * @return the parsed result.
         */
        template <class AstFactory, class Extension>
        static result parse(Source& source, const AstFactory& ast_factory, const Extension& extension) noexcept {
            //tokenize
            typename lexer_type::result lexer_result = lexer_type::parse(source);

            //parse the tokens
            ParserGrammar grammar;
            using parse_context_type = parse_context<typename lexer_type::parsed_token_container_type, node_id_type, error_id_type, case_sensitive_comparator, Extension>;
            parse_context_type pc(lexer_result.parsed_tokens, extension);
            const bool success = grammar.parse(pc) && pc.is_end_parse_position();

            //create the ast nodes
            ast_node_container_type ast_nodes;
            for (const auto& m : pc.matches()) {
                ast_nodes.push_back(create_ast_node(m, ast_factory));
            }

            error_container_type errors;

            //translate the lexer errors to parser errors
            if constexpr (parser_grammar_has_translate_lexer_error_id<ParserGrammar>::value) {
                for (const auto& lexer_error : lexer_result.errors) {
                    errors.push_back(error_type(ParserGrammar::translate_lexer_error_id(lexer_error.id()), lexer_error.begin(), lexer_error.end()));
                }
            }

            //if the whole input is not consumed, then add an error to the parsed position
            if constexpr (grammar_has_incomplete_parse_error_id<ParserGrammar>::value) {
                if (!pc.is_end_parse_position()) {
                    pc.add_error(ParserGrammar::incomplete_parse_error_id, pc.parse_position(), lexer_result.parsed_tokens.end());
                }
            }

            //translate the errors to the result errors; the result errors shall contain source positions, not token positions
            for (const auto& error : pc.errors()) {
                if (error.begin() != lexer_result.parsed_tokens.end()) {
                    errors.push_back(error_type(error.id(), error.begin()->begin(), std::prev(error.end())->end()));
                }
                else {
                    errors.push_back(error_type(error.id(), source.end(), source.end()));
                }
            }

            //sort the errors
            pc.sort_errors();

            //return the result
            return { success && errors.empty(), ast_nodes, errors };
        }

        /**
         * Parse function with two parameters.
         * @param source the source to parse.
         * @param ASTFactoryOrExtension either an AST factory or a parse context extension.
         *  The class of it must inherit either from class ast_factory_base or class parse_context_extension_base or both.
         * @return the parse result.
         */
        template <class ASTFactoryOrExtension>
        static result parse(Source& source, const ASTFactoryOrExtension& ast_factory_or_extension) noexcept {
            static_assert(std::is_base_of_v<ast_factory_base, ASTFactoryOrExtension> || std::is_base_of_v<parse_context_extension_base, ASTFactoryOrExtension>, "parserlib: parser: parse(source, ast_factory_or_extension): parameter 'ast_factory_or_extension' is neither an ast factory nor a parse context extension.");

            if constexpr (std::is_base_of_v<ast_factory_base, ASTFactoryOrExtension> && std::is_base_of_v<parse_context_extension_base, ASTFactoryOrExtension>) {
                return parse(source, ast_factory_or_extension, ast_factory_or_extension);
            }

            else if constexpr (std::is_base_of_v<ast_factory_base, ASTFactoryOrExtension> && !std::is_base_of_v<parse_context_extension_base, ASTFactoryOrExtension>) {
                return parse(source, ast_factory_or_extension, empty_parse_context_extension());
            }

            else if constexpr (!std::is_base_of_v<ast_factory_base, ASTFactoryOrExtension> && std::is_base_of_v<parse_context_extension_base, ASTFactoryOrExtension>) {
                return parse(source, default_ast_factory(), ast_factory_or_extension);
            }
        }

        /**
         * Parse function with one parameter.
         * The default ast factory is used, and an empty parse context extension.
         * @param source the source to parse.
         * @return the parse result.
         */
        static result parse(Source& source) noexcept {
            return parse(source, default_ast_factory(), empty_parse_context_extension());
        }

    private:
        template <class Match, class AstFactory>
        static ast_node_ptr_type create_ast_node(const Match& match, const AstFactory& ast_factory) noexcept {
            ast_node_container_type children;
            for (const auto& child_match : match.children()) {
                children.push_back(create_ast_node(child_match, ast_factory));
            }
            return ast_factory(match.id(), match.begin()->begin(), std::prev(match.end())->end(), std::move(children));
        }

        template <class AstFactory, class Extension>
        static result parse_helper(Source& source, const AstFactory& ast_factory, const Extension& extension) noexcept {
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSER_HPP
