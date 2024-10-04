#ifndef PARSERLIB_CFE_TOKENIZE_HPP
#define PARSERLIB_CFE_TOKENIZE_HPP


#include <algorithm>
#include "parserlib/core/ParseContext.hpp"


namespace parserlib::cfe {


    template <class Source, class Grammar, class TokenContainer, class ErrorContainer>
    bool tokenize(Source& input, Grammar&& grammar, TokenContainer& tokens, ErrorContainer& errors) {
        typedef typename TokenContainer::value_type Token;
        typedef typename Token::TokenID TokenID;

        typedef core::ParseContext<TokenID, Source> ParseContext;

        //parse
        ParseContext pc(input);
        bool success = grammar.parse(pc) && pc.isEndPosition();

        //get tokens
        for (const auto& match : pc.getMatches()) {
            tokens.push_back(Token(match.getID(), match.getStartPosition(), match.getEndPosition()));
        }

        typedef typename ErrorContainer::value_type Error;

        //get errors
        for (const auto& error : pc.getErrors()) {
            errors.push_back(Error(error.getID(), error.getStartPosition(), error.getEndPosition()));
        }
        std::sort(errors.begin(), errors.end());

        return success;
    }


} //namespace parserlib::cfe


#endif //PARSERLIB_CFE_TOKENIZE_HPP
