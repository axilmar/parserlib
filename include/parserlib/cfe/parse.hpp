#ifndef PARSERLIB_CFE_PARSE_HPP
#define PARSERLIB_CFE_PARSE_HPP


#include <utility>
#include "parserlib/core/ParseContext.hpp"
#include "AST.hpp"


namespace parserlib::cfe {


    namespace detail {


        template <class ASTContainer, class Match, class CreateASTFunc> 
        void createASTHelper(ASTContainer& ast, const Match& match, CreateASTFunc&& createAST) {
            ASTContainer children;
            for (const auto& childMatch : match.getChildren()) {
                createASTHelper(children, childMatch, std::forward<CreateASTFunc>(createAST));
            }
            ast.push_back(createAST(match.getID(), match.getStartPosition(), match.getEndPosition(), std::move(children)));
        }


        template <class ASTContainer> 
        struct DefaultASTCreation {
            typedef typename ASTContainer::value_type ASTPtr;
            typedef typename ASTPtr::element_type AST;
            typedef typename AST::ASTID ASTID;

            typedef typename AST::Source TokenContainer;
            typedef typename TokenContainer::const_iterator TokenIterator;

            static ASTPtr createAST(const ASTID& id, const TokenIterator& startPosition, const TokenIterator& endPosition, ASTContainer&& children) {
                return std::make_shared<AST>(id, startPosition, endPosition, std::forward<ASTContainer>(children));
            }
        };


    } //namespace detail


    template <class TokenContainer, class Grammar, class ASTContainer, class ErrorContainer, class CreateASTFunc>
    bool parse(TokenContainer& input, Grammar&& grammar, ASTContainer& ast, ErrorContainer& errors, CreateASTFunc&& createAST) {
        typedef typename ASTContainer::value_type ASTPtr;
        typedef typename ASTPtr::element_type AST;
        typedef typename AST::ASTID ASTID;
        typedef core::ParseContext<ASTID, TokenContainer> ParseContext;

        //parse
        ParseContext pc(input);
        bool success = grammar.parse(pc) && pc.isEndPosition();

        //create the ast
        for (const auto& match : pc.getMatches()) {
            detail::createASTHelper(ast, match, std::forward<CreateASTFunc>(createAST));
        }

        typedef typename ErrorContainer::value_type Error;

        //create the errors
        for (const auto& error : pc.getErrors()) {
            errors.push_back(Error(error.getID(), error.getStartPosition(), error.getEndPosition()));
        }
        std::sort(errors.begin(), errors.end());

        return success;
    }


    template <class TokenContainer, class Grammar, class ASTContainer, class ErrorContainer>
    bool parse(TokenContainer& input, Grammar&& grammar, ASTContainer& ast, ErrorContainer& errors) {
        return parse(input, std::forward<Grammar>(grammar), ast, errors, &detail::DefaultASTCreation<ASTContainer>::createAST);
    }


} //namespace parserlib::cfe


#endif //PARSERLIB_CFE_PARSE_HPP
