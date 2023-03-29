#ifndef PARSERLIB_UTIL_HPP
#define PARSERLIB_UTIL_HPP


#include <tuple>
#include <string>
#include <fstream>
#include <sstream>


namespace parserlib {


    /**
     * Generic implementation of a function to be invoked at scope exit.
     * @param F type of function to invoke.
     */
    template <class F> class RAII {
    public:
        /**
         * Constructor.
         * @param func function to invoke at exit.
         */
        RAII(const F& func) : m_func(func) {
        }

        /**
         * Invokes the function.
         */
        ~RAII() {
            m_func();
        }

    private:
        F m_func;
    };


    /**
     * Utility function for loading an ASCII file.
     * @param filename name of the file.
     * @return the file as a string.
     */
    inline std::string loadASCIIFile(const char* filename) {
        std::ifstream strm(filename);
        std::stringstream buffer;
        buffer << strm.rdbuf();
        return buffer.str();
    }


} //namespace parserlib


#endif //PARSERLIB_UTIL_HPP
