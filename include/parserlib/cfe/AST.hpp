#ifndef PARSERLIB_CFE_AST_HPP
#define PARSERLIB_CFE_AST_HPP


#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <ostream>
#include <sstream>
#include "Token.hpp"


namespace parserlib::cfe {


    /**
     * An Abtract syntax tree node.
     * 
     * @param ASTID id of AST node.
     * 
     * @param Source source of AST node.
     */
    template <class ASTID = int, class Source = TokenContainer<>> 
    class AST : public std::enable_shared_from_this<AST<ASTID, Source>> {
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
         */
        AST(const ASTID& id, const SourceIterator& startPosition, const SourceIterator& endPosition)
            : m_id(id)
            , m_startPosition(startPosition)
            , m_endPosition(endPosition)
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
         * Returns a pointer to the parent node.
         * @return pointer to parent node.
         */
        ASTPtr getParent() const {
            return m_parent.lock();
        }

        /**
         * Adds a child AST node.
         * @param child child node to add.
         */
        void addChild(const ASTPtr& child) {
            m_children.push_back(child);
            child->m_parent = this->shared_from_this();
        }

        /**
         * Removes a child node.
         * @param child child to remove.
         */
        void removeChild(const ASTPtr& child) {
            auto it = std::find(m_children.begin(), m_children.end(), child);
            m_children.erase(it);
            child->m_parent.reset();
        }

        /**
         * Removes this node from its parent, if there is one.
         */
        void detach() {
            ASTPtr parent = getParent();
            if (parent) {
                parent->removeChild(this->shared_from_this());
            }
        }

        /**
         * Removes all children. 
         */
        void removeChildren() {
            for (const ASTPtr& child : m_children) {
                child->m_parent.reset();
            }
            m_children.clear();
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

            //print the header
            stream << toString(maxSourceCharsPerLine);

            stream << '\n';

            //print children
            for (const ASTPtr& child : m_children) {
                child->print(stream, depth + 1, tabSize, maxSourceCharsPerLine);
            }
        }

        /**
         * Convert node to string (without children).
         * @param maxSourceCharsPerLine maximum number of characters from source code to print per line.
         * @return string.
         */
        std::basic_string<typename TokenSource::value_type> toString(size_t maxSourceCharsPerLine = 32) const {
            std::basic_stringstream<typename TokenSource::value_type> stream;
            stream << m_id;
            if (maxSourceCharsPerLine > 0 && m_children.empty()) {
                stream << ": " << getSource(maxSourceCharsPerLine);
            }
            return stream.str();
        }

    private:
        ASTID m_id;
        SourceIterator m_startPosition;
        SourceIterator m_endPosition;
        ASTContainer m_children;
        std::weak_ptr<ASTClass> m_parent;
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
