#ifndef PARSERLIB_ERROR_HPP
#define PARSERLIB_ERROR_HPP


#include <cassert>
#include "parse_definitions.hpp"
#include "span.hpp"


namespace parserlib {


    template <class ParseDefinitions = parse_definitions<>>
    class error : public ParseDefinitions {
    public:
        using error_id_type = typename ParseDefinitions::error_id_type;
        using input_span_type = typename ParseDefinitions::input_span_type;
        using input_iterator_type = typename ParseDefinitions::input_iterator_type;

        error() 
            : m_id()
            , m_span()
            , m_valid(false)
        {
        }

        error(const error_id_type& id, const input_span_type& span, const input_iterator_type& error_pos) noexcept
            : m_id(id)
            , m_span(span)
            , m_error_pos(error_pos)
            , m_valid(true)
        {
            assert(error_pos >= span.begin() && error_pos < span.end());
        }

        const error_id_type& id() const noexcept {
            assert(m_valid);
            return m_id;
        }

        const input_span_type& span() const noexcept {
            assert(m_valid);
            return m_span;
        }

        const input_iterator_type& position() const noexcept {
            return m_error_pos;
        }

        bool valid() const noexcept {
            return m_valid;
        }

    private:
        error_id_type m_id;
        input_span_type m_span;
        input_iterator_type m_error_pos;
        bool m_valid;
    };


} //namespace parserlib


#endif //PARSERLIB_ERROR_HPP
