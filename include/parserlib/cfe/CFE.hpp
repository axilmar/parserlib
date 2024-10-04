#ifndef PARSERLIB_CFE_CFE_HPP
#define PARSERLIB_CFE_CFE_HPP


#include "tokenize.hpp"
#include "parse.hpp"
#include "Token.hpp"
#include "AST.hpp"
#include "parserlib/core/ParseError.hpp"
#include "parserlib/core/Rule.hpp"


namespace parserlib::cfe {


    template <class TokenID = int, class ASTID = int, class Source = core::SourceString<>> 
    class CFE {
    public:
        typedef TokenID TokenID;

        typedef ASTID ASTID;

        typedef Source Source;

        typedef cfe::Token<TokenID, Source> Token;

        typedef cfe::TokenContainer<TokenID, Source> TokenContainer;

        typedef std::unique_ptr<TokenContainer> TokenContainerPtr;

        typedef core::ParseContext<TokenID, Source> TokenizerParseContext;

        typedef core::Rule<TokenizerParseContext> TokenizerRule;

        typedef cfe::AST<ASTID, TokenContainer> AST;

        typedef cfe::ASTPtr<ASTID, TokenContainer> ASTPtr;

        typedef cfe::ASTContainer<ASTID, TokenContainer> ASTContainer;

        typedef std::unique_ptr<ASTContainer> ASTContainerPtr;

        typedef core::ParseContext<ASTID, TokenContainer> ParserParseContext;

        typedef core::Rule<ParserParseContext> ParserRule;

        typedef core::ParseError<Source> Error;

        typedef core::ParseErrorContainer<Source> ErrorContainer;

        typedef std::unique_ptr<ErrorContainer> ErrorContainerPtr;

        template <class TokenGrammar, class ASTGrammar, class CreateAstFunc>
        static std::tuple<bool, TokenContainerPtr, ASTContainerPtr, ErrorContainerPtr> 
        parse(Source& input, TokenGrammar&& tokenGrammar, ASTGrammar&& astGrammar, CreateAstFunc&& createAST) {
            //tokenize
            TokenContainerPtr tokens = std::make_unique<TokenContainer>();
            ErrorContainerPtr errors = std::make_unique<ErrorContainer>();
            bool tokenizeSuccess = cfe::tokenize(input, std::forward<TokenGrammar>(tokenGrammar), *tokens, *errors);

            //parse
            ASTContainerPtr ast = std::make_unique<ASTContainer>();
            core::ParseErrorContainer<TokenContainer> astErrors;
            bool parseSuccess = cfe::parse(*tokens, std::forward<ASTGrammar>(astGrammar), *ast, astErrors, std::forward<CreateAstFunc>(createAST));

            //convert ast errors to source errors
            for (const auto& astError : astErrors) {
                errors->push_back(Error(astError.getID(), astError.getStartPosition()->getStartPosition(), std::prev(astError.getEndPosition())->getEndPosition()));
            }

            //sort all errors base on position
            std::sort(errors->begin(), errors->end());

            //return success, tokens, ast, errors
            return std::make_tuple(tokenizeSuccess && parseSuccess, std::move(tokens), std::move(ast), std::move(errors));
        }

        template <class TokenGrammar, class ASTGrammar>
        static std::tuple<bool, TokenContainerPtr, ASTContainerPtr, ErrorContainerPtr>
            parse(Source& input, TokenGrammar&& tokenGrammar, ASTGrammar&& astGrammar)
        {
            return parse(
                input, 
                std::forward<TokenGrammar>(tokenGrammar), 
                std::forward<ASTGrammar>(astGrammar), 
                &detail::DefaultASTCreation<ASTContainer>::createAST);
        }
    };


} //namespace parserlib::cfe


#endif //PARSERLIB_CFE_CFE_HPP
