#ifndef PARSERLIB_UTIL_HPP
#define PARSERLIB_UTIL_HPP


#include <cctype>


namespace parserlib {


    class Less {
    public:
        template <class A, class B>
        bool operator ()(const A& a, const B& b) const {
            return a < b;
        }
    };


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


    template <class Char>
    inline Char toLowerCase(Char c) {
        return (Char)std::tolower((int)c);
    }


} //namespace parserlib


#endif //PARSERLIB_UTIL_HPP
