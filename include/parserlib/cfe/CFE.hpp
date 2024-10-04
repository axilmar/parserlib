#ifndef PARSERLIB_CFE_CFE_HPP
#define PARSERLIB_CFE_CFE_HPP


#include "tokenize.hpp"
#include "parse.hpp"
#include "Token.hpp"
#include "AST.hpp"
#include "parserlib/core/ParseError.hpp"
#include "parserlib/core/Rule.hpp"


namespace parserlib::cfe {


    /**
     * A compiler front end that uses the given token id, ast id and source type.
     * 
     * @param TokenID id of tokens.
     * 
     * @param ASTID id of AST nodes.
     * 
     * @param Source source to parse.
     */
    template <class TokenID = int, class ASTID = int, class Source = core::SourceString<>> 
    class CFE {
    public:
        /**
         * Token ID.
         */
        typedef TokenID TokenID;

        /**
         * AST ID.
         */
        typedef ASTID ASTID;

        /**
         * Source type.
         */
        typedef Source Source;

        /**
         * Token type.
         */
        typedef cfe::Token<TokenID, Source> Token;

        /**
         * Token container type.
         */
        typedef cfe::TokenContainer<TokenID, Source> TokenContainer;

        /**
         * Token container ptr.
         */
        typedef std::unique_ptr<TokenContainer> TokenContainerPtr;

        /**
         * Parse context for tokenizer.
         */
        typedef core::ParseContext<TokenID, Source> TokenizerParseContext;

        /**
         * Rule for tokenizer.
         */
        typedef core::Rule<TokenizerParseContext> TokenizerRule;

        /**
         * AST type.
         */
        typedef cfe::AST<ASTID, TokenContainer> AST;

        /**
         * AST ptr type.
         */
        typedef cfe::ASTPtr<ASTID, TokenContainer> ASTPtr;

        /**
         * AST container type.
         */
        typedef cfe::ASTContainer<ASTID, TokenContainer> ASTContainer;

        /**
         * AST container ptr type.
         */
        typedef std::unique_ptr<ASTContainer> ASTContainerPtr;

        /**
         * Parser parse context.
         */
        typedef core::ParseContext<ASTID, TokenContainer> ParserParseContext;

        /**
         * Parser rule.
         */
        typedef core::Rule<ParserParseContext> ParserRule;

        /**
         * Error type over source.
         */
        typedef core::ParseError<Source> Error;

        /**
         * Error container type.
         */
        typedef core::ParseErrorContainer<Source> ErrorContainer;

        /**
         * Error container ptr type.
         */
        typedef std::unique_ptr<ErrorContainer> ErrorContainerPtr;

        /**
         * Parses a source into tokens, then parses the tokens into an AST.
         * @param input source code.
         * @param tokenGrammar the tokenizer grammar.
         * @param astGrammar the AST grammar.
         * @param createAST function to create an AST node; can be used to override node creation.
         * @return success flag, tokens, ast nodes, errors.
         */
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

        /**
         * Parses a source into tokens, then parses the tokens into an AST.
         * It uses a default AST creation function which creates a standard AST node.
         * @param input source code.
         * @param tokenGrammar the tokenizer grammar.
         * @param astGrammar the AST grammar.
         * @return success flag, tokens, ast nodes, errors.
         */
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
