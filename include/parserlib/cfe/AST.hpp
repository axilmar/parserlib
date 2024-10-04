#ifndef PARSERLIB_CFE_AST_HPP
#define PARSERLIB_CFE_AST_HPP


#include <vector>
#include <memory>
#include <utility>
#include <ostream>
#include "Token.hpp"


namespace parserlib::cfe {


    /**
     * An Abtract syntax tree node.
     * 
     * @param ASTID id of AST node.
     * 
     * @param Source source of AST node.
     */
    template <class ASTID = int, class Source = TokenContainer<>> class AST {
    public:
        /**
         * The AST id type.
         */
        typedef ASTID ASTID;

        /**
         * The AST source.
         */
        typedef Source Source;

        /**
         * The source iterator.
         */
        typedef typename Source::const_iterator SourceIterator;

        /**
         * The type of this class.
         */
        typedef AST<ASTID, Source> ASTClass;

        /**
         * Pointer to AST.
         */
        typedef std::shared_ptr<ASTClass> ASTPtr;

        /**
         * Container of AST nodes.
         */
        typedef std::vector<ASTPtr> ASTContainer;

        /**
         * Token type.
         */
        typedef typename Source::value_type Token;

        /**
         * Token source.
         */
        typedef typename Token::Source TokenSource;

        /**
         * Source string view.
         */
        typedef std::basic_string_view<typename TokenSource::value_type> StringView;

        /**
         * The constructor.
         * @param id the id of this AST node.
         * @param startPosition start position of this AST node into the token container.
         * @param endPosition end position of this AST node into the token container.
         * @param children the children AST nodes.
         */
        AST(const ASTID& id, const SourceIterator& startPosition, const SourceIterator& endPosition, ASTContainer&& children)
            : m_id(id)
            , m_startPosition(startPosition)
            , m_endPosition(endPosition)
            , m_children(std::move(children))
        {
        }

        /**
         * Returns the id.
         * @return the id.
         */
        const ASTID& getID() const {
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
         * Returns the children nodes.
         * @return the children nodes.
         */
        const ASTContainer& getChildren() const {
            return m_children;
        }

        /**
         * Returns the source code for this AST node.
         * @param maxChars max chars to return.
         * @return the source code for this AST node.
         */
        StringView getSource(size_t maxChars = -1) const {
            const auto* src = &*m_startPosition->getStartPosition();
            const size_t size = std::min(maxChars, (size_t)(std::prev(m_endPosition)->getEndPosition() - m_startPosition->getStartPosition()));
            return StringView(src, size);
        }

        /**
         * Prints this AST tree.
         * @param stream output stream.
         * @param depth print depth.
         * @param tabSize number of characters per tab.
         * @param maxSourceCharsPerLine maximum number of characters from source code to print per line.
         */
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


    /**
     * AST pointer type.
     */
    template <class ASTID = int, class Source = TokenContainer<>>
    using ASTPtr = std::shared_ptr<AST<ASTID, Source>>;


    /**
     * AST container type.
     */
    template <class ASTID = int, class Source = TokenContainer<>>
    using ASTContainer = std::vector<ASTPtr<ASTID, Source>>;


} //namespace parserlib::cfe


#endif //PARSERLIB_CFE_AST_HPP
