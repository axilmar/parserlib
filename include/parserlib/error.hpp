#ifndef PARSERLIB_ERROR_HPP
#define PARSERLIB_ERROR_HPP


#include <cassert>
#include "parse_definitions.hpp"
#include "span.hpp"


namespace parserlib {


    /**
     * A parse error.
     * @param ParseDefinitions parse definitions.
     */
    template <class ParseDefinitions = parse_definitions<>>
    class error : public ParseDefinitions {
    public:
        /** Error id type. */
        using error_id_type = typename ParseDefinitions::error_id_type;

        /** Input span type. */
        using input_span_type = typename ParseDefinitions::input_span_type;

        /** Input iterator type. */
        using input_iterator_type = typename ParseDefinitions::input_iterator_type;

        /**
         * The default constructor.
         * The object is constructed in an invalid state.
         */
        error() 
            : m_id()
            , m_span()
            , m_valid(false)
        {
        }

        /**
         * The valid object constructor.
         * @param id id of the error.
         * @param span the span of the error in the source.
         * @param error_pos the position the error starts from.
         */
        error(const error_id_type& id, const input_span_type& span, const input_iterator_type& error_pos) noexcept
            : m_id(id)
            , m_span(span)
            , m_error_pos(error_pos)
            , m_valid(true)
        {
            assert(error_pos >= span.begin() && error_pos < span.end());
        }

        /**
         * Returns the id of the error.
         * @return the id of the error.
         */
        const error_id_type& id() const noexcept {
            assert(m_valid);
            return m_id;
        }

        /**
         * Returns the span of the error.
         * @return the span of the error.
         */
        const input_span_type& span() const noexcept {
            assert(m_valid);
            return m_span;
        }

        /**
         * Returns the error position.
         * @return the error position.
         */
        const input_iterator_type& position() const noexcept {
            return m_error_pos;
        }

        /**
         * Returns the validity flag.
         * @return the validity flag.
         */
        bool valid() const noexcept {
            return m_valid;
        }

        /**
         * Returns true if the error is invalid.
         * @return true if the error is invalid, false otherwise.
         */
        bool invalid() const noexcept {
            return !m_valid;
        }

    private:
        error_id_type m_id;
        input_span_type m_span;
        input_iterator_type m_error_pos;
        bool m_valid;
    };


} //namespace parserlib


#endif //PARSERLIB_ERROR_HPP
