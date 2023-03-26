#ifndef PARSERLIB_UTIL_HPP
#define PARSERLIB_UTIL_HPP


#include <tuple>
#include <string>
#include <fstream>
#include <sstream>


namespace parserlib {


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
