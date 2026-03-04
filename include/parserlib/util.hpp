#ifndef PARSERLIB_UTIL_HPP
#define PARSERLIB_UTIL_HPP


#include <string>
#include <fstream>


namespace parserlib {


    template <class Elem, class Traits, class Alloc>
    static void trim_string_end(std::basic_string<Elem, Traits, Alloc>& str) {
        while (str.size() > 0) {
            const auto ch = str.back();
            if (std::isspace(str.back()) || ch < ' ') {
                str.resize(str.size() - 1);
                continue;
            }
            break;
        }
    }


    std::string load_text_file(const char* filename) {
        std::ifstream file(filename, std::ios::ate);
        file.exceptions(std::ios_base::badbit);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::string result;
        result.resize(size);
        file.read(result.data(), size);
        trim_string_end(result);
        return result;
    }


} //namespace parserlib


#endif //PARSERLIB_UTIL_HPP
