#ifndef PARSERLIB_PARSE_CONTEXT_DEFAULT_OPTIONS_HPP
#define PARSERLIB_PARSE_CONTEXT_DEFAULT_OPTIONS_HPP


#include <string>


namespace parserlib {


    /**
     * The default text position is empty.
     * It does not provide any line and column counting.
     */
    class default_text_position {
    public:
        /**
         * Returns the line.
         * @return always 0.
         */
        size_t line() const {
            return 0;
        }

        /**
         * Returns the column.
         * @return always 0.
         */
        size_t column() const {
            return 0;
        }

        /**
         * Increments the column by one.
         * Does nothing.
         */
        void increment_column() {
        }

        /**
         * Increments the column by a specific amount.
         * Does nothing.
         * @param count count to increment the column by.
         */
        void increment_column(size_t count) {
        }

        /**
         * Increments the line and sets the column to its initial value.
         * Does nothing.
         */
        void increment_line() {
        }

        /**
         * Returns an empty string,
         * since the default text position does not keep line and column.
         * @return an empty string.
         */
        std::string to_string() const {
            return {};
        }
    };


    /**
     * The default symbol comparator returns the difference between two operands,
     * without any changes in the values.
     */
    class default_symbol_comparator {
    public:
        /**
         * Returns a - b without any conversion.
         * @param a the first operand.
         * @param b the second operand.
         * @return their difference.
         */
        int operator ()(int a, int b) const {
            return a - b;
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_DEFAULT_OPTIONS_HPP
