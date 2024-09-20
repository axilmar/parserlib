#ifndef PARSERLIB_CFE_HPP
#define PARSERLIB_CFE_HPP


#include <memory>
#include <algorithm>
#include "ParseContext.hpp"


namespace parserlib {


    template <class TokenType, class ASTType, class SourceType = std::string, bool CaseSensitive = true, class NewlineTraits = DefaultNewlineTraits> class CFE {
    public:
        class ASTNode;

        using ASTNodePtr = std::shared_ptr<ASTNode>;

        using ASTNodeContainerType = std::vector<ASTNodePtr>;

        using SourcePositionType = LineCountingSourcePosition<SourceType, CaseSensitive, NewlineTraits>;

        using ErrorContainerType = ErrorContainer<SourcePositionType>;

        class ASTNode {
        public:
            ASTNode(const ASTType& id, const SourcePositionType& begin, const SourcePositionType& end, std::vector<ASTNodePtr>&& children) 
                : m_id(id), m_begin(begin), m_end(end), m_children(std::move(children))
            {
            }

            virtual ~ASTNode() {
            }

            ASTType id() const {
                return m_id;
            }

            const SourcePositionType& begin() const {
                return m_begin;
            }

            const SourcePositionType& end() const {
                return m_end;
            }

            const std::vector<ASTNodePtr>& children() const {
                return m_children;
            }

        private:
            ASTType m_id;
            SourcePositionType m_begin;
            SourcePositionType m_end;
            std::vector<ASTNodePtr> m_children;
        };

        template <class LexerGrammar, class ParserGrammar>
        std::tuple<bool, ASTNodeContainerType, ErrorContainerType> 
        parse(const SourceType& source, const LexerGrammar& lexerGrammar, const ParserGrammar& parserGrammar)
        {
            //parse tokens
            ParseContext<SourceType, TokenType, SourcePositionType> tokenizeParseContext(source);
            const bool tokenizeSuccess = lexerGrammar(tokenizeParseContext) && tokenizeParseContext.sourceEnded();

            //create token list
            std::vector<Token> tokens;
            for (const auto& tokenMatch : tokenizeParseContext.matches()) {
                tokens.push_back(Token{ tokenMatch.id(), tokenMatch.begin(), tokenMatch.end(), tokenMatch.content()});
            }

            //parse tokens
            ParseContext<std::vector<Token>, ASTType, TokenPosition> parseContext(tokens);
            const bool parseSuccess = parserGrammar(parseContext) && parseContext.sourceEnded();

            //create AST nodes
            std::vector<ASTNodePtr> astNodes;
            for (const ASTMatchType& astMatch : parseContext.matches()) {
                astNodes.push_back(createASTNode(astMatch));
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

    protected:
        struct Token {
            TokenType id;
            SourcePositionType begin;
            SourcePositionType end;
            SourceType content;
            operator TokenType () const { return id; }
        };

        using TokenPosition = SourcePosition<std::vector<Token>>;

        using ASTMatchType = Match<std::vector<Token>, ASTType, TokenPosition>;

        virtual ASTNodePtr createASTNode(const ASTMatchType& match) {
            return std::make_shared<ASTNode>(match.id(), match.begin().begin()->begin, std::prev(match.end().begin())->end, getChildren(match));
        }

    private:
        std::vector<ASTNodePtr> getChildren(const ASTMatchType& match) {
            std::vector<ASTNodePtr> children;
            for (const ASTMatchType& child : match.children()) {
                children.push_back(createASTNode(child));
            }
            return children;
        }
    };


} //namespace parserlib


#endif //PARSERLIB_CFE_HPP
