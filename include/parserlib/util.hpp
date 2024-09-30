#ifndef PARSERLIB_UTIL_HPP
#define PARSERLIB_UTIL_HPP


#include <cctype>
#include <fstream>
#include <sstream>


namespace parserlib {


    /**
     * Predicate to use for comparing different types.
     */
    class Less {
    public:
        /**
         * Comparison operator.
         * @param a 1st element to compare.
         * @param b 2nd element to compare.
         * @return a < b.
         */
        template <class A, class B>
        bool operator ()(const A& a, const B& b) const {
            return a < b;
        }
    };


    /**
     * Returns the length of a null-terminated string,
     * independendly of the string type.
     * @param str null-terminated string to measure the length of.
     * @return the length of the given null-terminated string.
     */
    template <class Char>
    size_t getStringLength(const Char* str) {
        size_t result = 0;
        if (str) {
            for (; *str; ++str) {
                ++result;
            }
        }
        return result;
    }


    /**
     * Converts a character to lowercase,
     * making the appropriate casts as needed.
     * Internally, the function std::tolower is used.
     * @param c character to convert to lower case.
     * @return the character that has been converted to lowercase.
     */
    template <class Char>
    inline Char toLowerCase(Char c) {
        return (Char)std::tolower((int)c);
    }


    /**
     * Loads a text file into memory.
     * @param filename name of the file.
     * @return a string that contains the text.
     */
    inline std::string loadFile(const std::string& filename) {
        std::ifstream strm(filename);
        std::stringstream buffer;
        buffer << strm.rdbuf();
        return buffer.str();
    }


} //namespace parserlib


#endif //PARSERLIB_UTIL_HPP
