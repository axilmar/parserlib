#ifndef PARSERLIB_ERROR_HPP
#define PARSERLIB_ERROR_HPP


#include "source_partition.hpp"


namespace parserlib {

    
    template <class Iterator, class Id>
    class error : public source_partition<Iterator, Id> {
    public:
        using source_partition<Iterator, Id>::source_partition;
    };


} //namespace parserlib


#endif //PARSERLIB_ERROR_HPP
