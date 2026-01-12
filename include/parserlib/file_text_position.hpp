#ifndef PARSERLIB_FILE_TEXT_POSITION_HPP
#define PARSERLIB_FILE_TEXT_POSITION_HPP


#include <cstddef>
#include <string>
#include <sstream>


namespace parserlib {


    /**
     * A file text position.
     * It maintains a line and a column.
     */
    class file_text_position {
    public:
        /**
         * Returns the line.
         * @return the line.
         */
        size_t get_line() const noexcept {
            return m_line;
        }

        /**
         * Returns the column.
         * @return the column.
         */
        size_t get_column() const noexcept {
            return m_column;
        }

        /**
         * Increments the column.
         */ 
        void operator ++() noexcept {
            ++m_column;
        }

        /**
         * Increments the column by an amount.
         * @param count amount of columns to increment.
         */ 
        void operator +=(size_t count) noexcept {
            m_column += count;
        }

        /**
         * Increments the line.
         * The column is set to 1.
         */
        void increment_line() noexcept {
            ++m_line;
            m_column = 1;
        }

        /**
         * Converts the text position to a string.
         * @return a string for this text position.
         */ 
        std::string to_string() const {
            std::stringstream stream;
            stream << "line " << m_line << ", column " << m_column;
            return stream.str();
        }

    private:
        size_t m_line{ 1 };
        size_t m_column{ 1 };
    };


} //namespace parserlib


#endif //PARSERLIB_FILE_TEXT_POSITION_HPP
