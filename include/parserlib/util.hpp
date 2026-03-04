#ifndef PARSERLIB_UTIL_HPP
#define PARSERLIB_UTIL_HPP


#include <string>
#include <sstream>
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


    inline std::string load_text_file(const char* filename) {
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


    template <class T>
    inline T get_command_line_option(int argc, const char* argv[], const std::string& option, const T& default_value) {
        for (int index = 1; index < argc; ++index) {
            if (option == argv[index]) {
                if (index < argc - 1) {
                    const char* option_value_string = argv[index + 1];
                    T option_value;
                    std::stringstream stream;
                    stream << option_value_string;
                    stream >> option_value;
                    return option_value;
                }
                break;
            }
        }
        return default_value;
    }


    inline bool get_command_line_switch(int argc, const char* argv[], const std::string& switch_, bool default_value) {
        for (int index = 1; index < argc; ++index) {
            if (switch_ == argv[index]) {
                return true;
            }
        }
        return default_value;
    }


} //namespace parserlib


#endif //PARSERLIB_UTIL_HPP
