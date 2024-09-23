#ifndef PARSERLIB_CFE_HPP
#define PARSERLIB_CFE_HPP


#include <memory>
#include <algorithm>
#include <ostream>
#include "ParseContext.hpp"


namespace parserlib {


    /**
     * A compiler front end.
     * @param TokenType type of token id, usually an enumeration of token ids.
     * @param ASTType type of AST id, usually an enumeration of AST node ids.
     * @param SourceType type of source; by default, an std::string.
     * @param CaseSensitive if true, then tokenization is case-sensitive, otherwise it is not.
     * @param NewlineTraits traits for newline; by default, it recognizes '\n' as a line separator.
     */
    template <class TokenType, class ASTType, class SourceType = std::string, bool CaseSensitive = true, class NewlineTraits = DefaultNewlineTraits> 
    class CFE {
    public:
        class ASTNode;

        /**
         * AST nodes are managed via shared pointers, because usually they need to be shared between other objects.
         */
        using ASTNodePtrType = std::shared_ptr<ASTNode>;

        /**
         * Container of AST nodes via pointers.
         */
        using ASTNodeContainerType = std::vector<ASTNodePtrType>;

        /**
         * The source position type counts lines and columns.
         */
        using SourcePositionType = LineCountingSourcePosition<SourceType, CaseSensitive, NewlineTraits>;

        /**
         * Error container type.
         */
        using ErrorContainerType = ErrorContainer<SourcePositionType>;

        /**
         * Token; structure created at tokenization and input to the parser.
         */
        struct Token {
            /**
             * Id of token.
             */
            const TokenType id;

            /**
             * Position into the source code that the token starts.
             */
            const SourcePositionType begin;

            /**
             * Position into the source code that the token ends (non-inclusive).
             */
            const SourcePositionType end;

            /**
             * Comparison with token type; required for parsing.
             * @param tokenType token type to compare this to.
             * @return true if the token types match, false otherwise.
             */
            bool operator == (const TokenType& tokenType) const {
                return id == tokenType;
            }
        };

        template <class T>
        friend std::basic_ostream<T, std::char_traits<T>>& operator << (std::basic_ostream<T, std::char_traits<T>>& stream, const Token& token) {
            //stream << SourceType(token.begin.iterator(), token.end.iterator());
            return stream;
        }

        /**
         * A token position is a simple source position.
         */
        using TokenPositionType = SourcePosition<std::vector<Token>>;

        /**
         * Type of parser match.
         */
        using ASTMatchType = Match<std::vector<Token>, ASTType, TokenPositionType>;

        /**
         * Base class for AST nodes.
         */
        class ASTNode {
        public:
            /**
             * Constructor from match.
             * It also creates children nodes from child matches.
             */
            template <class ASTNodeFactory>
            ASTNode(const ASTMatchType& match, const ASTNodeFactory& astNodeFactory) :
                ASTNode(match.id(), match.begin().begin()->begin, std::prev(match.end().begin())->end, getChildren(match, astNodeFactory))
            {
            }

            /**
             * Constructor from match attributes.
             * @param id AST node id.
             * @param begin start of AST node in the source.
             * @param end end of AST node in the source.
             * @param children children nodes.
             */
            ASTNode(const ASTType& id, const SourcePositionType& begin, const SourcePositionType& end, std::vector<ASTNodePtrType>&& children) 
                : m_id(id), m_begin(begin), m_end(end), m_children(std::move(children))
            {
            }

            /**
             * Virtual destructor due to inheritance.
             */
            virtual ~ASTNode() {
            }

            /**
             * Returns the type id of the node.
             * @return the type id of the node.
             */
            ASTType id() const {
                return m_id;
            }

            /**
             * Returns the start position of the node.
             * @return the start position of the node.
             */
            const SourcePositionType& begin() const {
                return m_begin;
            }

            /**
             * Returns the end position of the node, non-inclusive.
             * @return the end position of the node, non-inclusive.
             */
            const SourcePositionType& end() const {
                return m_end;
            }

            /**
             * Returns the children nodes.
             * @return the children nodes.
             */
            const std::vector<ASTNodePtrType>& children() const {
                return m_children;
            }

            /**
             * Returns a copy of the portion of the source 
             * that corresponds to this AST node.
             * @return the node's source.
             */
            SourceType getSource() const {
                return SourceType(m_begin.iterator(), m_end.iterator());
            }

            /**
             * Returns a copy of the portion of the source
             * that corresponds to this AST node, but newline characters are replaced with string '\n'.
             * @return the node's source.
             */
            SourceType getContent() const {
                return copyReplaceChars<SourceType>(m_begin.iterator(), m_end.iterator(), '\n', "\\n");
            }

            /**
             * Prints the node and its children, as a tree, in the given output stream.
             * @param ostream stream to print to.
             * @param depth current tree depth.
             * @param tabSize number of spaces per tab.
             */
            template <class T>
            void print(std::basic_ostream<T, std::char_traits<T>>&ostream, size_t depth = 0, size_t tabSize = 4) const {
                for (size_t t = depth * tabSize; t > 0; --t) {
                    ostream << ' ';
                }
                ostream << m_id << ": " << getContent() << '\n';
                for (const ASTNodePtrType& child : m_children) {
                    child->print(ostream, depth + 1);
                }
            }

        private:
            ASTType m_id;
            SourcePositionType m_begin;
            SourcePositionType m_end;
            std::vector<ASTNodePtrType> m_children;

            //create children recursively.
            template <class ASTNodeFactory>
            static std::vector<ASTNodePtrType> getChildren(const ASTMatchType& match, const ASTNodeFactory& astNodeFactory) {
                std::vector<ASTNodePtrType> children;
                for (const ASTMatchType& child : match.children()) {
                    children.push_back(astNodeFactory(child));
                }
                return children;
            }
        };

        /**
         * Lexer parse context type.
         */
        using LexerParseContextType = ParseContext<SourceType, TokenType, SourcePositionType>;

        /**
         * Lexer rule type.
         */
        using LexerRuleType = Rule<LexerParseContextType>;

        /**
         * Parse context used for parsing tokens into an AST.
         */
        using ASTParseContextType = ParseContext<std::vector<Token>, ASTType, TokenPositionType>;

        /**
         * Rule type for an AST parsing grammar.
         */
        using RuleType = Rule<ASTParseContextType>;

        /**
         * Static parse function which takes a lexer grammar, a parser grammar, a source,
         * and a AST node factory, and uses all that to a) tokenize the source, b) parse the tokens into an AST.
         * @param source source (i.e. the source code).
         * @param lexerGrammar the lexer grammar; e.g. it creates tokens.
         * @param parserGrammar the parser grammar; e.g. it creates AST nodes.
         * @param astNodeFactory factory class to create AST nodes; it can be overriden to provide extended AST nodes with more context if needed.
         * @return a tuple of: a) success/failure status, b) list of top-level AST nodes created, c) list of errors.
         *  The parsing is considered true if all the source is consumed and no errors are found;
         *  The list of errors is sorted first by line, then by column.
         */
        template <class LexerGrammar, class ParserGrammar, class ASTNodeFactory>
        static std::tuple<bool, ASTNodeContainerType, ErrorContainerType> 
        parse(const SourceType& source, const LexerGrammar& lexerGrammar, const ParserGrammar& parserGrammar, const ASTNodeFactory& astNodeFactory) {
            //parse tokens
            LexerParseContextType tokenizeParseContext(source);
            const bool tokenizeSuccess = lexerGrammar(tokenizeParseContext) && tokenizeParseContext.sourceEnded();

            //create token list
            std::vector<Token> tokens;
            for (const auto& tokenMatch : tokenizeParseContext.matches()) {
                tokens.push_back(Token{ tokenMatch.id(), tokenMatch.begin(), tokenMatch.end()});
            }

            //parse tokens
            ASTParseContextType parseContext(tokens);
            const bool parseSuccess = parserGrammar(parseContext) && parseContext.sourceEnded();

            //create AST nodes
            std::vector<ASTNodePtrType> astNodes;
            for (const ASTMatchType& astMatch : parseContext.matches()) {
                astNodes.push_back(astNodeFactory(astMatch));
            }

            //handle errors
            ErrorContainerType errors;
            for (const auto& tokenizerError : tokenizeParseContext.errors()) {
                errors.push_back(tokenizerError);
            }
            if (tokenizeParseContext.errors().empty()) {
                errors.push_back(makeError(ErrorType::SyntaxError, tokenizeParseContext.sourcePosition(), "Syntax error."));
            }
            for (const auto& parseError : parseContext.errors()) {
                errors.push_back(makeError(parseError.type(), parseError.position().begin()->begin, parseError.message()));
            }
            std::sort(errors.begin(), errors.end(), [](const Error<SourcePositionType>& a, const Error<SourcePositionType>& b) {
                if (a.position().line() < b.position().line()) {
                    return -1;
                }
                if (a.position().line() > b.position().line()) {
                    return 1;
                }
                if (a.position().column() < b.position().column()) {
                    return -1;
                }
                if (a.position().column() > b.position().column()) {
                    return 1;
                }
                return 0;
            });

            const bool success = tokenizeSuccess && parseSuccess;

            return std::make_tuple(success, astNodes, errors);
        }

        /**
         * Parse function which uses this class as an AST node factory.
         * @param lexerGrammar the lexer grammar; e.g. it creates tokens.
         * @param parserGrammar the parser grammar; e.g. it creates AST nodes.
         * @return same as the static version.
         */
        template <class LexerGrammar, class ParserGrammar>
        std::tuple<bool, ASTNodeContainerType, ErrorContainerType> 
        parse(const SourceType& source, const LexerGrammar& lexerGrammar, const ParserGrammar& parserGrammar) const {
            return parse(source, lexerGrammar, parserGrammar, *this);
        }

        /**
         * Operator to create an AST node.
         * @param match the parser match to create an AST node from.
         * @return a pointer to the created AST node.
         */
        ASTNodePtrType operator ()(const ASTMatchType& match) const {
            return std::make_shared<ASTNode>(match, *this);
        }
    };


} //namespace parserlib


#endif //PARSERLIB_CFE_HPP
