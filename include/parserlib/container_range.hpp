#ifndef PARSERLIB_CONTAINER_RANGE_HPP
#define PARSERLIB_CONTAINER_RANGE_HPP


namespace parserlib {


    template <class It> class container_range {
    public:
        container_range(const It& begin = It(), const It& end = It())
            : m_begin(begin)
            , m_end(end)
        {
        }

        const It& begin() const {
            return m_begin;
        }

        const It& end() const {
            return m_end;
        }

    private:
        const It m_begin;
        const It m_end;
    };


} //namespace parserlib


#endif //PARSERLIB_CONTAINER_RANGE_HPP
