#ifndef PARSERLIB_CFE_AST_HPP
#define PARSERLIB_CFE_AST_HPP


#include <vector>
#include <memory>
#include <utility>
#include <ostream>
#include "Token.hpp"


namespace parserlib::cfe {


    template <class ASTID = int, class Source = TokenContainer<>> class AST {
    public:
        typedef ASTID ASTID;

        typedef Source Source;

        typedef typename Source::const_iterator SourceIterator;

        typedef AST<ASTID, Source> ASTType;

        typedef std::shared_ptr<ASTType> ASTPtr;

        typedef std::vector<ASTPtr> ASTContainer;

        typedef typename Source::value_type Token;

        typedef typename Token::Source TokenSource;

        typedef std::basic_string_view<typename TokenSource::value_type> StringView;

        AST() {
        }

        AST(const ASTID& id, const SourceIterator& startPosition, const SourceIterator& endPosition, ASTContainer&& children)
            : m_id(id)
            , m_startPosition(startPosition)
            , m_endPosition(endPosition)
            , m_children(std::move(children))
        {
        }

        const ASTID& getID() const {
            return m_id;
        }

        const SourceIterator& getStartPosition() const {
            return m_startPosition;
        }

        const SourceIterator& getEndPosition() const {
            return m_endPosition;
        }

        const ASTContainer& getChildren() const {
            return m_children;
        }

        StringView getSource(size_t maxChars = -1) const {
            const auto* src = &*m_startPosition->getStartPosition();
            const size_t size = std::min(maxChars, (size_t)(std::prev(m_endPosition)->getEndPosition() - m_startPosition->getStartPosition()));
            return StringView(src, size);
        }

        template <class CharT, class CharTraits>
        void print(std::basic_ostream<CharT, CharTraits>& stream, size_t depth = 0, size_t tabSize = 4, size_t maxSourceCharsPerLine = 32) const {
            //print depth
            for (size_t i = 0; i < depth * tabSize; ++i) {
                stream << ' ';
            }

            //print the ast id
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
        ASTID m_id;
        SourceIterator m_startPosition;
        SourceIterator m_endPosition;
        ASTContainer m_children;
    };


    template <class ASTID, class Source>
    using ASTPtr = std::shared_ptr<AST<ASTID, Source>>;


    template <class ASTID, class Source>
    using ASTContainer = std::vector<ASTPtr<ASTID, Source>>;


} //namespace parserlib::cfe


#endif //PARSERLIB_CFE_AST_HPP
