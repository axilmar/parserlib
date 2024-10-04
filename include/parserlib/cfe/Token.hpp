#ifndef PARSERLIB_CFE_TOKEN_HPP
#define PARSERLIB_CFE_TOKEN_HPP


#include <vector>
#include <string_view>
#include <ostream>
#include "parserlib/core/SourceString.hpp"


namespace parserlib::cfe {


    /**
     * A token.
     * 
     * It is created from tokenizing an input, and it is fed to a parser to parse a language grammar.
     * 
     * @param TokenID id of token.
     * 
     * @param Source source for the token.
     */
    template <class TokenID = int, class Source = core::SourceString<>> class Token {
    public:
        /**
         * Token id type.
         */
        typedef TokenID TokenID;

        /**
         * Source type.
         */
        typedef Source Source;

        /**
         * Source iterator type.
         */
        typedef typename Source::const_iterator SourceIterator;

        /**
         * Source view type.
         */
        typedef std::basic_string_view<typename Source::value_type> StringView;

        /**
         * Constructor.
         * @param id id of token.
         * @param startPosition start position of the token into the source.
         * @param endPosition end position of the the token into the source.
         */
        Token(const TokenID& id, const SourceIterator& startPosition, const SourceIterator& endPosition)
            : m_id(id)
            , m_startPosition(startPosition)
            , m_endPosition(endPosition)
        {
        }

        /**
         * Returns the token id.
         * @return the token id.
         */
        const TokenID& getID() const {
            return m_id;
        }

        /**
         * Returns the start position.
         * @return the start position.
         */
        const SourceIterator& getStartPosition() const {
            return m_startPosition;
        }

        /**
         * Returns the end position.
         * @return the end position.
         */
        const SourceIterator& getEndPosition() const {
            return m_endPosition;
        }

        /**
         * Returns the source that correponds to this token.
         * @return the source that correponds to this token.
         */
        StringView getSource(size_t maxChars = -1) const {
            const auto* src = &*m_startPosition;
            const size_t size = std::min(maxChars, (size_t)(m_endPosition - m_startPosition));
            return StringView(src, size);
        }

        /**
         * Checks if this token has the given id.
         * @param id id to check.
         * @return true if this token has the given id, false otherwise.
         */
        bool operator == (const TokenID& id) const {
            return m_id == id;
        }

        /**
         * Checks if this token does not have the given id.
         * @param id id to check.
         * @return true if this token does not have the given id, false otherwise.
         */
        bool operator != (const TokenID& id) const {
            return m_id != id;
        }

    private:
        TokenID m_id;
        SourceIterator m_startPosition;
        SourceIterator m_endPosition;
    };
   

    /**
     * Token container type.
     */
    template <class TokenID = int, class Source = core::SourceString<>>
    using TokenContainer = std::vector<Token<TokenID, Source>>;


    /**
     * Outputs a token to an output stream.
     * @param stream output stream.
     * @param token token to send to the output stream.
     * @return the stream.
     */
    template <class Elem, class Traits, class TokenID, class Source>
    std::basic_ostream<Elem, Traits>& 
    operator << (std::basic_ostream<Elem, Traits>& stream, const Token<TokenID, Source>& token) {
        stream << token.getSource();
        return stream;
    }


} //namespace parserlib::cfe


#endif //PARSERLIB_CFE_TOKEN_HPP
