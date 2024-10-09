#ifndef PARSERLIB_CFE_TOKENIZE_HPP
#define PARSERLIB_CFE_TOKENIZE_HPP


#include <algorithm>
#include "parserlib/core/ParseContext.hpp"
#include "parserlib/core/ParseErrorType.hpp"


namespace parserlib::cfe {


    /**
     * Tokenizes an input.
     * @param input the input to tokenize.
     * @param grammar the grammar to use for the tokenizer.
     * @param tokens the output tokens.
     * @param errors the output errors.
     * @return true on success, false on failure.
     */
    template <class Source, class Grammar, class TokenContainer, class ErrorContainer>
    bool tokenize(Source& input, Grammar&& grammar, TokenContainer& tokens, ErrorContainer& errors) {
        typedef typename TokenContainer::value_type Token;
        typedef typename Token::TokenID TokenID;
        typedef core::ParseContext<TokenID, Source> ParseContext;
        typedef typename ErrorContainer::value_type Error;

        //parse
        ParseContext pc(input);
        bool success = grammar.parse(pc) && pc.isEndPosition();

        //add error if end of source has not been reached
        if (!pc.isEndPosition()) {
            errors.push_back(Error((int)core::ParseErrorType::SyntaxError, pc.getCurrentPosition(), pc.getEndPosition()));
        }

        //get tokens
        for (const auto& match : pc.getMatches()) {
            tokens.push_back(Token(match.getID(), match.getStartPosition(), match.getEndPosition()));
        }

        //get errors
        for (const auto& error : pc.getErrors()) {
            errors.push_back(Error(error.getID(), error.getStartPosition(), error.getEndPosition()));
        }
        std::sort(errors.begin(), errors.end());

        return success;
    }


} //namespace parserlib::cfe


#endif //PARSERLIB_CFE_TOKENIZE_HPP
