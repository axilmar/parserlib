#ifndef PARSERLIB_UTILITIES_HPP
#define PARSERLIB_UTILITIES_HPP


#include <string>
#include <fstream>
#include <ios>
#include <stdexcept>


namespace parserlib {


    template <class Char = char>
    std::basic_string<Char> load_file(const std::string& filename) {
        //declare the filestream
        std::ifstream filestream(filename, std::ios::binary | std::ios::ate);

        //get its size, in bytes
        const std::streamsize byte_size = filestream.tellg();

        //reset the file pointer to the beginning of the file.
        filestream.seekg(0, std::ios::beg);

        //compute character size
        const std::streamsize char_size = byte_size / sizeof(Char);

        //allocate memory for the result
        std::basic_string<Char> result;
        result.resize(byte_size);

        //read the data
        if (filestream.read(reinterpret_cast<char*>(result.data()), byte_size)) {
            return result;
        }

        //error
        throw std::runtime_error("parserlib: load_file: File could not be loaded.");
    }


} //namespace parserlib


#endif //PARSERLIB_UTILITIES_HPP
