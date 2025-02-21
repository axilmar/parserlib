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
        using input_span_type = typename ParseDefinitions::input_span_type;

        using id_type = typename ParseDefinitions::output_token_type;
        using match_type = match<ParseDefinitions>;
        using match_container_type = std::vector<match_type>;

        match()
            : m_id()
            , m_span()
            , m_children()
        {
        }

        match(const id_type& id, const input_span_type& span, match_container_type&& children) noexcept
            : m_id(id)
            , m_span(span)
            , m_children(std::move(children))
        {
        }

        const id_type& id() const noexcept {
            return m_id;
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

        template <class OutputStream, class GetIdName>
        void print(OutputStream& stream, const GetIdName& get_id_name, std::size_t depth = 0, std::size_t tabSize = 4) const {
            for (std::size_t i = 0; i < depth * tabSize; ++i) {
                stream << ' ';
            }
            stream << get_id_name(m_id);
            if (m_children.empty()) {
                stream << " : " << source() << '\n';
            }
            else {
                stream << '\n';
                for (const match<ParseDefinitions>& childMatch : m_children) {
                    childMatch.print(stream, get_id_name, depth + 1, tabSize);
                }
            }
        }

    private:
        id_type m_id;
        input_span_type m_span;
        match_container_type m_children;
    };


} //namespace parserlib


#endif //PARSERLIB_MATCH_HPP
