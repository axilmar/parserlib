#ifndef PARSERLIB_UTILITY_HPP
#define PARSERLIB_UTILITY_HPP


#include <fstream>
#include <sstream>
#include <string>
#include <cstddef>
#include <vector>


namespace parserlib {


    /**
     * Loads a file into memory.
     * @param path path to the file.
     * @param dst string to place the file's contents.
     * @return true if the file was loaded successfully, false otherwise.
     */
    template <class PathChar, class PathTraits, class PathAlloc, class DstChar, class DstTraits, class DstAlloc>
    bool load_file(const std::basic_string<PathChar, PathTraits, PathAlloc>& path, std::basic_string<DstChar, DstTraits, DstAlloc>& dst) {
        std::basic_ostringstream<DstChar, DstTraits> buffer;
        std::basic_ifstream<DstChar, DstTraits> input(path.c_str());
        buffer << input.rdbuf();
        dst = buffer.str();
        return input.good();
    }


} //namespace parserlib


#endif //PARSERLIB_UTILITY_HPP
