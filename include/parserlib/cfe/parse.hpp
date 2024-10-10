#ifndef PARSERLIB_CFE_PARSE_HPP
#define PARSERLIB_CFE_PARSE_HPP


#include <utility>
#include "parserlib/core/ParseContext.hpp"
#include "parserlib/core/ParseErrorType.hpp"
#include "AST.hpp"


namespace parserlib::cfe {


    namespace detail {


        template <class ASTPtr, class Match, class CreateASTFunc> 
        void createChildASTHelper(const ASTPtr& parent, const Match& match, CreateASTFunc&& createAST) {
            auto astNode = createAST(match.getID(), match.getStartPosition(), match.getEndPosition());
            parent->addChild(astNode);
            for (const auto& childMatch : match.getChildren()) {
                createChildASTHelper(astNode, childMatch, std::forward<CreateASTFunc>(createAST));
            }
        }


        template <class ASTContainer, class Match, class CreateASTFunc>
        void createASTHelper(ASTContainer& astContainer, const Match& match, CreateASTFunc&& createAST) {
            auto astNode = createAST(match.getID(), match.getStartPosition(), match.getEndPosition());
            astContainer.push_back(astNode);
            for (const auto& childMatch : match.getChildren()) {
                createChildASTHelper(astNode, childMatch, std::forward<CreateASTFunc>(createAST));
            }
        }


        template <class ASTContainer>
        struct DefaultASTCreation {
            typedef typename ASTContainer::value_type ASTPtr;
            typedef typename ASTPtr::element_type AST;
            typedef typename AST::ASTID ASTID;

            typedef typename AST::Source TokenContainer;
            typedef typename TokenContainer::const_iterator TokenIterator;

            static ASTPtr createAST(const ASTID& id, const TokenIterator& startPosition, const TokenIterator& endPosition) {
                return std::make_shared<AST>(id, startPosition, endPosition);
            }
        };


    } //namespace detail


    /**
     * Parses a token container in order to create an AST.
     * @param input the tokens to parse.
     * @param grammar the grammar to use for parsing.
     * @param ast the container of AST nodes.
     * @param errors container to receive the errors.
     * @param createAST function to use for AST creation; same signature as AST constructor.
     * @return true on success, false on error.
     */
    template <class TokenContainer, class Grammar, class ASTContainer, class ErrorContainer, class CreateASTFunc>
    bool parse(TokenContainer& input, Grammar&& grammar, ASTContainer& ast, ErrorContainer& errors, CreateASTFunc&& createAST) {
        typedef typename ASTContainer::value_type ASTPtr;
        typedef typename ASTPtr::element_type AST;
        typedef typename AST::ASTID ASTID;
        typedef core::ParseContext<ASTID, TokenContainer> ParseContext;
        typedef typename ErrorContainer::value_type Error;

        //parse
        ParseContext pc(input);
        bool success = grammar.parse(pc) && pc.isEndPosition();

        //add error if end of source has not been reached
        if (!pc.isEndPosition()) {
            errors.push_back(Error((int)core::ParseErrorType::SyntaxError, pc.getFurthestUnparsedPosition(), pc.getEndPosition()));
        }

        //create the ast
        for (const auto& match : pc.getMatches()) {
            detail::createASTHelper(ast, match, std::forward<CreateASTFunc>(createAST));
        }

        //create the errors
        for (const auto& error : pc.getErrors()) {
            errors.push_back(Error(error.getID(), error.getStartPosition(), error.getEndPosition()));
        }
        std::sort(errors.begin(), errors.end());

        return success;
    }


    /**
     * Parses a token container in order to create an AST.
     * It uses a default function for AST creation.
     * @param input the tokens to parse.
     * @param grammar the grammar to use for parsing.
     * @param ast the container of AST nodes.
     * @param errors container to receive the errors.
     * @return true on success, false on error.
     */
    template <class TokenContainer, class Grammar, class ASTContainer, class ErrorContainer>
    bool parse(TokenContainer& input, Grammar&& grammar, ASTContainer& ast, ErrorContainer& errors) {
        return parse(input, std::forward<Grammar>(grammar), ast, errors, &detail::DefaultASTCreation<ASTContainer>::createAST);
    }


} //namespace parserlib::cfe


#endif //PARSERLIB_CFE_PARSE_HPP
