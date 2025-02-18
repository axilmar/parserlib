#ifndef PARSERLIB_SPAN_HPP
#define PARSERLIB_SPAN_HPP


#include <cassert>
#include <algorithm>


namespace parserlib {


    template <class Iterator> 
    class span {
    public:
        using input_iterator_type = Iterator;

        span()
            : m_begin()
            , m_end()
        {
        }

        span(const Iterator& begin, const Iterator& end) noexcept
            : m_begin(begin)
            , m_end(end)
        {
            assert(begin <= end);
        }

        const Iterator& begin() const noexcept {
            return m_begin;
        }

        const Iterator& end() const noexcept {
            return m_end;
        }

        auto distance() const noexcept {
            return std::distance(m_begin, m_end);
        }

    private:
        Iterator m_begin;
        Iterator m_end;
    };


} //namespace parserlib


#endif //PARSERLIB_SPAN_HPP
