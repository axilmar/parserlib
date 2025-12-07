#ifndef PARSERLIB_INVALID_GENERIC_SOURCE_ITERATOR_HPP
#define PARSERLIB_INVALID_GENERIC_SOURCE_ITERATOR_HPP


#include <stdexcept>
#include <string>


namespace parserlib {


    class invalid_generic_source_iterator : public std::runtime_error {
    public:
        invalid_generic_source_iterator(const std::string& msg = "invalid generic source iterator")
            : std::runtime_error(msg)
        {
        }
    };


} //namespace parserlib


#endif //PARSERLIB_INVALID_GENERIC_SOURCE_ITERATOR_HPP

