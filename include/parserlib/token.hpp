#ifndef PARSERLIB_TOKEN_HPP
#define PARSERLIB_TOKEN_HPP


#include "span.hpp"


namespace parserlib {


    /**
     * A token.
     * @param Input input type.
     * @param TokenID token id type.
     */
    template <class Input, class TokenID>
    class token {
    public:
        /** Input type. */
        using input_type = Input;

        /** Input iterator type. */
        using input_iterator_type = typename input_type::const_iterator;

        /** Input span type. */
        using input_span_type = span<input_iterator_type>;

        /** token id type. */
        using token_id_type = TokenID;

        /**
         * Constructor.
         * @param id id of token.
         * @param span span of token.
         */
        token(const token_id_type& id = token_id_type(), const input_span_type& span = input_span_type())
            : m_id(id)
            , m_span(span)
        {
        }

        /**
         * Returns the id of the token.
         * @return id the id of the token.
         */
        const token_id_type& id() const {
            return m_id;
        }

        /**
         * Returns the span of the token.
         * @return the span of the token.
         */
        const input_span_type& span() const {
            return m_span;
        }

        /**
         * Required comparator for parsing a container of tokens.
         * @param tk token.
         * @param id token id.
         * @return the numeric difference between the token's id and the given id.
         */
        friend int operator - (const token& tk, const token_id_type& id) {
            return static_cast<int>(tk.m_id) - static_cast<int>(id);
        }

    private:
        token_id_type m_id;
        input_span_type m_span;
    };


} //namespace parserlib


#endif //PARSERLIB_TOKEN_HPP
