#ifndef PARSERLIB_SOURCE_PARTITION_HPP
#define PARSERLIB_SOURCE_PARTITION_HPP


#include <algorithm>


namespace parserlib {


    template <class Iterator, class Id>
    class source_partition {
    public:
        using iterator_type = Iterator;
        using id_type = Id;

        source_partition(const Id& id = {}, const Iterator& begin = {}, const Iterator& end = {})
            : m_id(id)
            , m_begin(begin)
            , m_end(end)
        {
        }

        const Id& get_id() const {
            return m_id;
        }

        const Iterator& begin() const {
            return m_begin;
        }

        const Iterator& end() const {
            return m_end;
        }

        auto get_source() const;

        size_t get_size() const {
            return static_cast<size_t>(std::distance(m_begin, m_end));
        }

    private:
        Id m_id;
        Iterator m_begin;
        Iterator m_end;
    };


} //namespace parserlib


#endif //PARSERLIB_SOURCE_PARTITION_HPP
