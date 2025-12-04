#ifndef PARSERLIB_PARSE_ERROR_HPP
#define PARSERLIB_PARSE_ERROR_HPP


#include "source_range.hpp"


namespace parserlib {


    template <class It, class Id, class TextPosition> class parse_error : public source_range<It, Id, TextPosition> {
    public:
        using source_range<It, Id, TextPosition>::source_range;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_ERROR_HPP
