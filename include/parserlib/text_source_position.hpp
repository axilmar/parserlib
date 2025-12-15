#ifndef PARSERLIB_TEXT_SOURCE_POSITION_HPP
#define PARSERLIB_TEXT_SOURCE_POSITION_HPP


#include <cstddef>


namespace parserlib {


    /**
     * A text source position.
     * It maintains a line and a column.
     * Both start counting from 1.
     */
    class text_source_position {
    public:
        /**
         * Returns the current line.
         * @return the current line.
         */
        std::size_t get_line() const {
            return m_line;
        }

        /**
         * Returns the current column.
         * @return the current column.
         */
        std::size_t get_column() const {
            return m_line;
        }

        /**
         * Increments the current column.
         */
        void increment() {
            ++m_column;
        }

        /**
         * Increments the current column by the given count.
         * @param count number of columns to add.
         */
        void increment(std::size_t count) {
            m_column += count;
        }

        /**
         * Increments the line and sets the column to 1.
         */
        void increment_line() {
            ++m_line;
            m_column = 1;
        }

    private:
        std::size_t m_line{ 1 };
        std::size_t m_column{ 1 };
    };


} //namespace parserlib


#endif //PARSERLIB_TEXT_SOURCE_POSITION_HPP
