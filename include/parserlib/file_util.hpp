#ifndef PARSERLIB_FILE_UTIL_HPP
#define PARSERLIB_FILE_UTIL_HPP


#include <fstream>
#include <sstream>
#include <string>


namespace parserlib {


    /**
     * Loads a file into memory.
     * @param path path to the file.
     * @return string that contains the file's contents.
     */
    std::string load_file(const std::string& path) {
        std::ostringstream buffer;
        std::ifstream input(path.c_str());
        buffer << input.rdbuf();
        return buffer.str();
    }


} //namespace parserlib


#endif //PARSERLIB_FILE_UTIL_HPP
