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


    template <class Source>
    struct source_position {
        typename Source::const_iterator iterator;
        std::size_t line;
        std::size_t column;
    };


    /**
     * Retrieves the line and column for each character in a source.
     * @param source source to get the line and column of.
     * @param delim line delimiter; by default, the character '\n'.
     * @return buffer with positions.
     */
    template <class Source>
    std::vector<source_position<Source>> get_source_positions(Source& source, char delim = '\n') {
        std::vector<source_position<Source>> result;

        std::size_t line = 1, column = 1;
        
        for (auto it = source.cbegin(); it != source.cend(); ++it) {
            if (*it == delim) {
                ++line;
                column = 1;
            }

            result.push_back(source_position<Source>{it, line, column});
            
            ++column;
        }

        return result;
    }


} //namespace parserlib


#endif //PARSERLIB_UTILITY_HPP
