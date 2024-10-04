#ifndef PARSERLIB_CFE_TOKEN_HPP
#define PARSERLIB_CFE_TOKEN_HPP


#include <vector>
#include <string_view>
#include "parserlib/core/SourceString.hpp"


namespace parserlib::cfe {


    template <class TokenID = int, class Source = core::SourceString<>> class Token {
    public:
        typedef TokenID TokenID;

        typedef Source Source;

        typedef typename Source::const_iterator SourceIterator;

        typedef std::basic_string_view<typename Source::value_type> StringView;

        Token(const TokenID& id, const SourceIterator& startPosition, const SourceIterator& endPosition)
            : m_id(id)
            , m_startPosition(startPosition)
            , m_endPosition(endPosition)
        {
        }

        const TokenID& getID() const {
            return m_id;
        }

        const SourceIterator& getStartPosition() const {
            return m_startPosition;
        }

        const SourceIterator& getEndPosition() const {
            return m_endPosition;
        }

        StringView getSource(size_t maxChars = -1) const {
            const auto* src = &*m_startPosition;
            const size_t size = std::min(maxChars, (size_t)(m_endPosition - m_startPosition));
            return StringView(src, size);
        }

        bool operator == (const TokenID& id) const {
            return m_id == id;
        }

        bool operator != (const TokenID& id) const {
            return m_id != id;
        }

    private:
        TokenID m_id;
        SourceIterator m_startPosition;
        SourceIterator m_endPosition;
    };
   

    template <class TokenID, class Source>
    using TokenContainer = std::vector<Token<TokenID, Source>>;


} //namespace parserlib::cfe


#endif //PARSERLIB_CFE_TOKEN_HPP
