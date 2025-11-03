#ifndef PARSERLIB_PARSE_CONTEXT_OPTIONS_HPP
#define PARSERLIB_PARSE_CONTEXT_OPTIONS_HPP


#include <string>
#include <cctype>


namespace parserlib {


    /**
     * A text position object that counts lines and columns.
     * Both values start from 1, in order to match text editors.
     */
    class text_position {
    public:
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

        /**
         * Increments the column.
         */
        void increment_column() {
            ++m_column;
        }

        /**
         * Increments the column by a specific amount of positions.
         * @param count number of positions to increment the column.
         */
        void increment_column(size_t count) {
            m_column += count;
        }

        /**
         * Increments the line and sets the column to its initial value.
         */
        void increment_line() {
            ++m_line;
            m_column = 1;
        }

        /**
         * Returns the string "line L, column C"
         * @return a string.
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


    /**
     * The case insensitive symbol comparator returns the difference between two operands,
     * by converting both operands to lower case, using std::tolower.
     */
    class case_insensitive_symbol_comparator {
    public:
        /**
         * Returns std::tolower(a) - std::tolower(b).
         * @param a the first operand.
         * @param b the second operand.
         * @return their difference.
         */
        int operator ()(int a, int b) const {
            return std::tolower(a) - std::tolower(b);
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_OPTIONS_HPP
