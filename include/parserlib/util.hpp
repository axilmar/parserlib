#ifndef PARSERLIB__UTIL__HPP
#define PARSERLIB__UTIL__HPP


#include <string>
#include <fstream>
#include <sstream>


namespace parserlib {


    /**
     * Utility function for loading an ASCII file.
     * @param filename name of the file.
     * @return the file as a string.
     */
    inline std::string load_file(const char* filename) {
        std::ifstream strm(filename);
        std::stringstream buffer;
        buffer << strm.rdbuf();
        return buffer.str();
    }


} //namespace parserlib


#endif //PARSERLIB__UTIL__HPP
