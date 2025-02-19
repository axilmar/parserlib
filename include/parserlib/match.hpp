#ifndef PARSERLIB_MATCH_HPP
#define PARSERLIB_MATCH_HPP


#include <vector>
#include <string_view>
#include <utility>
#include "parse_definitions.hpp"


namespace parserlib {


    template <class ParseDefinitions = parse_definitions<>> 
    class match : public ParseDefinitions {
    public:
        using input_iterator_type = typename ParseDefinitions::input_iterator_type;
        using input_token_type = typename ParseDefinitions::input_token_type;

        using output_token_type = typename ParseDefinitions::output_token_type;
        using input_span_type = typename ParseDefinitions::input_span_type;

        using span_type = span<input_iterator_type>;

        using match_type = match<ParseDefinitions>;
        using match_container_type = std::vector<match_type>;

        match()
            : m_token()
            , m_span()
            , m_children()
        {
        }

        match(const output_token_type& token, const input_span_type& span, match_container_type&& children) noexcept
            : m_token(token)
            , m_span(span)
            , m_children(std::move(children))
        {
        }

        const output_token_type& token() const noexcept {
            return m_token;
        }

        const input_span_type& span() const noexcept {
            return m_span;
        }

        const match_container_type& children() const noexcept {
            return m_children;
        }

        auto source() const noexcept {
            if constexpr (std::is_trivial_v<input_token_type>) {
                return std::basic_string_view<input_token_type>(m_span.data(), m_span.size());
            }
            else {
                return std::vector<input_token_type>(m_span.begin(), m_span.end());
            }
        }

    private:
        output_token_type m_token;
        input_span_type m_span;
        match_container_type m_children;
    };


} //namespace parserlib


#endif //PARSERLIB_MATCH_HPP
