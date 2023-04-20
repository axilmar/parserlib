#ifndef PARSERLIB_UTIL_HPP
#define PARSERLIB_UTIL_HPP


#include <string>
#include <fstream>
#include <sstream>


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


} //namespace parserlib


#endif //PARSERLIB_UTIL_HPP
