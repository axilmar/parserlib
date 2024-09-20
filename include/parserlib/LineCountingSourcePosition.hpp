#ifndef PARSERLIB_LINECOUNTINGSOURCEPOSITION_HPP
#define PARSERLIB_LINECOUNTINGSOURCEPOSITION_HPP


#include "SourcePosition.hpp"


namespace parserlib {


    /**
     * Default newline traits.
     */
    class DefaultNewlineTraits {
    public:
        /**
         * Checks if the character at the given position is '\n'.
         * @param it iterator to the source.
         * @param end iterator to the end of the source.
         * @return true if the character is '\n', false otherwise.
         */
        template <class Iterator> bool isNewline(const Iterator& it, const Iterator& /*end*/) const {
            return *it == '\n';
        }

        /**
         * Increments the iterator in order to skip the newline.
         * @param it iterator to the source.
         * @param end iterator to the end of the source.
         */
        template <class Iterator> void skipNewline(Iterator& it, const Iterator& /*end*/) const {
            ++it;
        }
    };


    /**
     * A source position that counts lines and columns.
     * @param SourceType source type.
     * @param CaseSensitive if true, comparison is case sensitive, otherwise case insensitive.
     * @param NewlineTraits traits for newline.
     */
    template <class SourceType = std::string, bool CaseSensitive = true, class NewlineTraits = DefaultNewlineTraits> 
    class LineCountingSourcePosition : public SourcePosition<SourceType, CaseSensitive>
    {
    public:
        /**
         * Newline traits.
         */
        using NewlineTraitsType = NewlineTraits;

        /**
         * The default constructor.
         */
        LineCountingSourcePosition() {
        }

        /**
         * Constructor.
         * @param begin iterator to the first element of the source.
         * @param end iterator to the end of the source.
         */
        LineCountingSourcePosition(const typename SourceType::const_iterator& begin, const typename SourceType::const_iterator& end)
            : SourcePosition<SourceType, CaseSensitive>(begin, end)
        {
        }

        /**
         * Increments the position by one place.
         * It also increments column/line, depending on if current sequence represents a newline.
         */
        void increment() {
            if (NewlineTraits().isNewline(SourcePosition<SourceType, CaseSensitive>::iterator(), SourcePosition<SourceType, CaseSensitive>::end())) {
                ++m_line;
                m_column = 1;
            }
            else {
                ++m_column;
            }
            SourcePosition<SourceType, CaseSensitive>::increment();
        }

        /**
         * Increases the position by multiple places.
         * It also increases the column value by the given amount.
         * @param count number of places to increase the position by.
         */
        void increase(size_t count) {
            m_column += count;
            SourcePosition<SourceType, CaseSensitive>::increase(count);
        }

        /**
         * Returns the line.
         * @return the line.
         */
        size_t line() const {
            return m_line;
        }

        /**
         * Returns the column.
         * @return the column.
         */
        size_t column() const {
            return m_column;
        }

    private:
        size_t m_line{1};
        size_t m_column{1};
    };


} //namespace parserlib


#endif //PARSERLIB_LINECOUNTINGSOURCEPOSITION_HPP
