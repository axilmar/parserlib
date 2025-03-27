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

        /** Input token type. */
        using input_token_type = typename ParseDefinitions::input_token_type;

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
         */
        error(const error_id_type& id, const input_span_type& span)
            : m_id(id)
            , m_span(span)
            , m_valid(true)
        {
        }

        /**
         * Returns the id of the error.
         * @return the id of the error.
         */
        const error_id_type& id() const {
            assert(m_valid);
            return m_id;
        }

        /**
         * Returns the span of the error.
         * @return the span of the error.
         */
        const input_span_type& span() const {
            assert(m_valid);
            return m_span;
        }

        /**
         * Returns the validity flag.
         * @return the validity flag.
         */
        bool valid() const {
            return m_valid;
        }

        /**
         * Returns true if the error is invalid.
         * @return true if the error is invalid, false otherwise.
         */
        bool invalid() const {
            return !m_valid;
        }

        /**
         * Returns the source that this node corresponds to.
         * If the input token is trivial, then it returns an std::basic_string_view,
         * otherwise it returns a vector that is copied from the source.
         * @return the source that this node corresponds to.
         */
        auto source() const {
            return m_span.source();
        }

    private:
        error_id_type m_id;
        input_span_type m_span;
        bool m_valid;
    };


} //namespace parserlib


#endif //PARSERLIB_ERROR_HPP
