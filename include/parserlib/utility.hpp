#ifndef PARSERLIB_UTILITY_HPP
#define PARSERLIB_UTILITY_HPP


#include <fstream>
#include <sstream>
#include <string>
#include <cstddef>
#include <vector>
#include <cctype>
#include <locale>


namespace parserlib {


    /**
     * Loads a file into memory.
     * @param path path to the file.
     * @param dst string to place the file's contents.
     * @return true if the file was loaded successfully, false otherwise.
     */
    template <class Path, class Dst>
    bool load_file(const Path& path, Dst& dst) {
        using value_type = typename Dst::value_type;
        using traits_type = std::char_traits<value_type>;
        std::basic_ostringstream<value_type, traits_type> buffer;
        std::basic_ifstream<value_type, traits_type> input(path.c_str());
        buffer << input.rdbuf();
        dst = buffer.str();
        return input.good();
    }


    /**
     * Loads a file into memory.
     * @param path path to the file.
     * @param dst string to place the file's contents.
     * @return true if the file was loaded successfully, false otherwise.
     */
    template <class Char, class Dst>
    bool load_file(const Char* path, Dst& dst) {
        return load_file(std::basic_string<Char>(path), dst);
    }


    /**
     * Checks if the given character is whitespace for the currently installed locale.
     * @param ch character.
     * @return true if the character is whitespace, false otherwise.
     */
    inline bool isspace(int ch) noexcept {
        return std::isspace(ch);
    }


    /**
     * Checks if the given character is a digit for the currently installed locale.
     * @param ch character.
     * @return true if the character is digit, false otherwise.
     */
    inline bool isdigit(int ch) noexcept {
        return std::isdigit(ch, std::locale(""));
    }


    /**
     * Checks if the given character is an alphabetic character for the currently installed locale.
     * @param ch character.
     * @return true if the character is an alphabetic character, false otherwise.
     */
    inline bool isalpha(int ch) noexcept {
        return std::isalpha(ch);
    }


    /**
     * Checks if the given character is an alphabetic character or digit for the currently installed locale.
     * @param ch character.
     * @return true if the character is an alphabetic character or digit, false otherwise.
     */
    inline bool isalnum(int ch) noexcept {
        return std::isalnum(ch);
    }


} //namespace parserlib


#endif //PARSERLIB_UTILITY_HPP
