#ifndef PARSERLIB_MATCH_HPP
#define PARSERLIB_MATCH_HPP


#include <vector>
#include "source_partition.hpp"


namespace parserlib {


    template <class Iterator, class Id>
    class match : public source_partition<Iterator, Id> {
    public:
        match(const Id& id = {}, const Iterator& begin = {}, const Iterator& end = {}, std::vector<match>&& children = {})
            : source_partition<Iterator, Id>(id, begin, end)
            , m_children(std::move(children))
        {
        }

        operator int () const {
            return static_cast<int>(this->get_id());
        }

        const std::vector<match>& get_children() const {
            return m_children;
        }

    private:
        std::vector<match> m_children;
    };


} //namespace parserlib


#endif //PARSERLIB_MATCH_HPP
