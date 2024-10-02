#ifndef PARSERLIB_CFE_HPP
#define PARSERLIB_CFE_HPP


#include <vector>
#include <tuple>
#include <memory>
#include <functional>
#include <utility>
#include <algorithm>
#include <ostream>
#include "SourceString.hpp"
#include "ParseError.hpp"
#include "ParseContext.hpp"
#include "Rule.hpp"


namespace parserlib {


    /**
     * A compiler front end.
     * @param TokenId type of token id.
     * @param ASTId type of AST id.
     * @param Source source type; SourceString, by default.
     */
    template <class TokenId, class ASTId, class Source = SourceString<>> class CFE {
    public:
        /**
         * Source type.
         */
        typedef Source Source;

        /**
         * Char type.
         */
        typedef typename Source::value_type CharType;

        /**
         * Char type compatible with STL containers.
         */
        typedef typename Source::value_type value_type;

        /**
         * Iterator type over source.
         */
        typedef typename Source::const_iterator Iterator;

        /**
         * Iterator type compatible with STL containers.
         */
        typedef typename Source::const_iterator const_iterator;

        /**
         * String view type.
         */
        typedef std::basic_string_view<value_type> StringView;

        /**
         * Token.
         */
        class Token {
        public:
            /**
             * The default constructor.
             * Members are default-initialized.
             */
            Token() {
            }

            /**
             * Constructor from parameters.
             * @param id id.
             * @param startPosition start position.
             * @param endPosition end position.
             */
            Token(const TokenId& id, const Iterator &startPosition, const Iterator& endPosition)
                : m_id(id)
                , m_startPosition(startPosition)
                , m_endPosition(endPosition)
            {
            }

            /**
             * Returns the id of the token.
             * @return the id of the token.
             */
            const TokenId& getId() const {
                return m_id;
            }

            /**
             * Returns the start position of the token.
             * @return the start position of the token.
             */
            const Iterator& getStartPosition() const {
                return m_startPosition;
            }

            /**
             * Returns the end position of the token.
             * @return the end position of the token.
             */
            const Iterator& getEndPosition() const {
                return m_endPosition;
            }

            /**
             * Checks if this token has the given id.
             * @param id id to check.
             * @return true if this token has the given id, false otherwise.
             */
            bool operator == (const TokenId& id) const {
                return m_id == id;
            }

            /**
             * Checks if this token does not have the given id.
             * @param id id to check.
             * @return true if this token does not have the given id, false otherwise.
             */
            bool operator != (const TokenId& id) const {
                return m_id != id;
            }

            /**
             * Returns a view over the source for this token.
             * @param maxChars maximum characters; if -1, the whole source that corresponds to this token is returned.
             * @return a string view for this token.
             */
            StringView getSource(size_t maxChars = -1) const {
                const CharType* src = &*m_startPosition;
                const size_t size = std::min(maxChars, (size_t)(m_endPosition - m_startPosition));
                return StringView(src, size);
            }

        private:
            TokenId m_id;
            Iterator m_startPosition;
            Iterator m_endPosition;
        };

        /**
         * Type of token container.
         */
        typedef std::vector<Token> TokenContainer;

        /**
         * Type of token iterator.
         */
        typedef typename TokenContainer::const_iterator TokenIterator;

        class AST;

        /**
         * AST nodes are managed via std::shared_ptr.
         */
        typedef std::shared_ptr<AST> ASTPtr;

        /**
         * Containers of AST nodes.
         */
        typedef std::vector<ASTPtr> ASTContainer;

        /**
         * An AST Node.
         */
        class AST {
        public:
            /**
             * The constructor.
             * @param id AST id.
             * @param startPosition start position into the source.
             * @param endPosition end position into the source.
             */
            AST(const ASTId& id, const Iterator& startPosition, const Iterator& endPosition, ASTContainer&& children)
                : m_id(id)
                , m_startPosition(startPosition)
                , m_endPosition(endPosition)
                , m_children(std::move(children))
            {
            }

            /**
             * The destructor.
             * Virtual due to possible inheritance.
             */
            virtual ~AST() {            
            }

            /**
             * Returns the id of this AST node.
             */
            const ASTId& getId() const {
                return m_id;
            }

            /**
             * Returns the start position of this node into the source.
             * @return the start position of this node into the source.
             */
            const Iterator& getStartPosition() const {
                return m_startPosition;
            }

            /**
             * Returns the end position of this node into the source.
             * @return the end position of this node into the source.
             */
            const Iterator& getEndPosition() const {
                return m_endPosition;
            }

            /**
             * Returns the container of children AST nodes.
             * @return the container of children AST nodes.
             */
            const ASTContainer& getChildren() const {
                return m_children;
            }

            /**
             * Returns a view over the source for this AST node.
             * @param maxChars maximum characters; if -1, the whole source that corresponds to this AST node is returned.
             * @return a string view for this AST node.
             */
            StringView getSource(size_t maxChars = -1) const {
                const CharType* src = &*m_startPosition;
                const size_t size = std::min(maxChars, (size_t)(m_endPosition - m_startPosition));
                return StringView(src, size);
            }

            /**
             * Prints this AST tree into the given stream.
             * @param stream output stream.
             * @param depth tree depth.
             * @param tabSize number of characters per tab.
             * @param maxSourceCharsPerLine max number of characters to print of the source per AST node.
             */
            template <class CharT, class CharTraits>
            void print(std::basic_ostream<CharT, CharTraits>& stream, size_t depth = 0, size_t tabSize = 4, size_t maxSourceCharsPerLine = 10) const {
                //print depth
                for (size_t i = 0; i < depth * tabSize; ++i) {
                    stream << ' ';
                }

                //print ast id
                stream << m_id;

                //print source
                if (maxSourceCharsPerLine > 0) {
                    stream << ": " << getSource(maxSourceCharsPerLine);
                }

                stream << '\n';

                //print children
                for (const ASTPtr& child : m_children) {
                    child->print(stream, depth + 1, tabSize, maxSourceCharsPerLine);
                }
            }

        private:
            ASTId m_id;
            Iterator m_startPosition;
            Iterator m_endPosition;
            ASTContainer m_children;
        };

        /**
         * Error type.
         */
        typedef ParseError<Source> Error;

        /**
         * Error container type.
         */
        typedef std::vector<Error> ErrorContainer;

        /**
         * Parse context for tokenization.
         */
        typedef ParseContext<TokenId, Source> TokenizerParseContext;

        /**
         * Rule for tokenization.
         */
        typedef Rule<TokenizerParseContext> TokenizerRule;

        /**
         * Parse context for parsing.
         */
        typedef ParseContext<ASTId, TokenContainer> ParserParseContext;

        /**
         * Rule for parsing.
         */
        typedef Rule<ParserParseContext> ParserRule;

        /**
         * Match for parser.
         */
        typedef typename ParserParseContext::Match ParserMatch;

        /**
         * Helper function to create an AST node.
         */
        static ASTPtr createAST(const ASTId& id, const Iterator& startPosition, const Iterator& endPosition, ASTContainer&& children) {
            return std::make_shared<AST>(id, startPosition, endPosition, std::move(children));
        }

        template <class TokenGrammar, class ASTGrammar, class ASTCreationFunc>
        static std::tuple<bool, ASTContainer, ErrorContainer> 
            parse(Source& source, TokenGrammar&& tokenGrammar, ASTGrammar&& astGrammar, ASTCreationFunc&& astCreationFunc)
        {
            ErrorContainer errors;

            //recognize tokens
            TokenizerParseContext tokenizerParseContext(source);
            bool tokenizationSuccess = tokenGrammar.parse(tokenizerParseContext) && tokenizerParseContext.isEndPosition();

            //create tokens
            TokenContainer tokens;
            for (const auto& match : tokenizerParseContext.getMatches()) {
                tokens.push_back(Token(match.getId(), match.getStartPosition(), match.getEndPosition()));
            }

            //get tokenization errors
            errors.insert(errors.end(), tokenizerParseContext.getErrors().begin(), tokenizerParseContext.getErrors().end());

            //parse tokens into an AST
            ParserParseContext parserParseContext(tokens);
            bool parsingSuccess = astGrammar.parse(parserParseContext) && parserParseContext.isEndPosition();

            //create AST
            ASTContainer ast;
            for (const auto& match : parserParseContext.getMatches()) {
                createASTTree(ast, match, std::forward<ASTCreationFunc>(astCreationFunc));
            }

            //get AST errors
            for (const ParseError<TokenContainer>& error : parserParseContext.getErrors()) {
                errors.push_back(Error(error.getId(), error.getStartPosition()->getStartPosition(), std::prev(error.getEndPosition())->getEndPosition()));
            }

            //sort all errors based on position
            std::sort(errors.begin(), errors.end(), [](const Error& a, const Error& b) {
                if (a.getStartPosition() < b.getStartPosition()) {
                    return -1;
                }
                if (a.getStartPosition() > b.getStartPosition()) {
                    return 1;
                }
                if (a.getEndPosition() < b.getEndPosition()) {
                    return -1;
                }
                if (a.getEndPosition() > b.getEndPosition()) {
                    return 1;
                }
                return 0;
                });

            //return success, ast and errors
            bool success = tokenizationSuccess && parsingSuccess;
            return std::make_tuple(success, std::move(ast), std::move(errors));
        }

        template <class TokenGrammar, class ASTGrammar>
        static std::tuple<bool, ASTContainer, ErrorContainer>
            parse(Source& source, TokenGrammar&& tokenGrammar, ASTGrammar&& astGrammar)
        {
            return parse(
                source, 
                std::forward<TokenGrammar>(tokenGrammar), 
                std::forward<ASTGrammar>(astGrammar), 
                &CFE<TokenId, ASTId, Source>::createAST);
        }

    private:
        template <class ASTCreationFunc>
        static void createASTTree(ASTContainer& ast, const ParserMatch& match, ASTCreationFunc&& astCreationFunc) {
            ASTContainer children;
            for (const ParserMatch& childMatch : match.getChildren()) {
                createASTTree(children, childMatch, std::forward<ASTCreationFunc>(astCreationFunc));
            }
            ast.push_back(astCreationFunc(match.getId(), match.getStartPosition()->getStartPosition(), std::prev(match.getEndPosition())->getEndPosition(), std::move(children)));
        }
    };


} //namespace parserlib


#endif //PARSERLIB_CFE_HPP
