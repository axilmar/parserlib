#ifndef PARSERLIB_ERROR_HPP
#define PARSERLIB_ERROR_HPP


#include <vector>
#include "source_partition.hpp"


namespace parserlib {

    
    template <class Iterator, class Id>
    class error : public source_partition<Iterator, Id> {
    public:
        using source_partition<Iterator, Id>::source_partition;
    };


    template <class OutputStream, class Iterator, class Id>
    void print_error(OutputStream& stream, const error<Iterator, Id>& e) {
        stream << "ERROR " << id_to_string(e.get_id()) << ": " << e.get_source() << '\n';
    }


    template <class OutputStream, class Iterator, class Id>
    void print_errors(OutputStream& stream, const std::vector<error<Iterator, Id>>& errors) {
        for (const error<Iterator, Id>& e : errors) {
            print_error(stream, e);
        }
    }


} //namespace parserlib


#endif //PARSERLIB_ERROR_HPP
