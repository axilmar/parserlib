#ifndef PARSERLIB_UTIL_HPP
#define PARSERLIB_UTIL_HPP


#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>


namespace parserlib {


    /**
     * Execute function at scope exit.
     * @param T type of function to execute.
     */
    template <class T> class ScopeExit {
    public:
        /**
         * Constructor.
         * @param f function.
         */
        ScopeExit(const T& f) : m_function(f) {
        }

        /**
         * Executes the function.
         */
        ~ScopeExit() {
            m_function();
        }

    private:
        const T m_function;
    };


    /**
     * Utility function for loading an ASCII file.
     * @param filename name of the file.
     * @param appendZero appends the '\0' character, optionally.
     * @return the file as a string.
     */
    inline std::string loadASCIIFile(const char* filename, const bool appendZero = false) {
        std::ifstream strm(filename);
        std::stringstream buffer;
        buffer << strm.rdbuf();
        if (appendZero) {
            buffer << '\0';
        }
        return buffer.str();
    }


    template <class Elem, class Traits, class T> 
    std::basic_ostream<Elem, Traits>& tokenToString(std::basic_ostream<Elem, Traits>& stream, const T& val) {
        stream << '\'' << val << '\'';
        return stream;
    }


    template <class T> std::string tokenToString(const T& val) {
        std::stringstream stream; 
        tokenToString(stream, val);
        return stream.str();
    }


    template <class Elem, class Traits, class T, class Alloc>
    std::basic_ostream<Elem, Traits>& operator << (std::basic_ostream<Elem, Traits>& stream, const std::vector<T, Alloc>& vec) {
        stream << '[';
        const char* c = "";
        for (const auto& v : vec) {
            stream << c;
            tokenToString(stream, v);
            c = ",";
        }
        stream << ']';
        return stream;
    }


    template <class... T> std::string toString(T&&... values) {
        std::stringstream stream;
        (stream << ... << std::forward<T>(values));
        return stream.str();
    }


    inline std::string toSubString(const std::string::const_iterator& begin, const std::string::const_iterator& end, size_t len) {
        return std::string(begin, begin + std::min(static_cast<std::ptrdiff_t>(len), std::distance(begin, end)));
    }


    template <class T>
    size_t stringLength(const T* string) {
        const T* s = string;
        for (; *s != '\0'; ++s) {}
        return s - string;
    }


} //namespace parserlib


#endif //PARSERLIB_UTIL_HPP
