#ifndef PARSERLIB_MATCH_HPP
#define PARSERLIB_MATCH_HPP


#include <vector>
#include <string_view>
#include <utility>
#include "parse_definitions.hpp"


namespace parserlib {


    /**
     * A parser match.
     * When an input is recognized, then the match parser adds a match to the context.
     * @param ParseDefinitions parse definitions.
     */
    template <class ParseDefinitions = parse_definitions<>> 
    class match : public ParseDefinitions {
    public:
        /** The input iterator type. */
        using input_iterator_type = typename ParseDefinitions::input_iterator_type;

        /** The input token type. */
        using input_token_type = typename ParseDefinitions::input_token_type;

        /** The input span type. */
        using input_span_type = typename ParseDefinitions::input_span_type;

        /**
         * The match id type.
         */
        using match_id_type = typename ParseDefinitions::output_token_type;

        /** The match type. */
        using match_type = match<ParseDefinitions>;

        /** The match container type. */
        using match_container_type = std::vector<match_type>;

        /** 
         * The empty match constructor.
         */
        match()
            : m_id()
            , m_span()
            , m_children()
        {
        }

        /**
         * Constructor from arguments.
         * @param id id of the match.
         * @param span span of the match.
         * @param children children matches.
         */
        match(const match_id_type& id, const input_span_type& span, match_container_type&& children) noexcept
            : m_id(id)
            , m_span(span)
            , m_children(std::move(children))
        {
        }

        /**
         * Returns the id of the match.
         * @return the id of the match.
         */
        const match_id_type& id() const noexcept {
            return m_id;
        }

        /**
         * Returns the span of the match.
         * @return the span of the match.
         */
        const input_span_type& span() const noexcept {
            return m_span;
        }

        /**
         * Returns the children matches.
         * @return the children matches.
         */
        const match_container_type& children() const noexcept {
            return m_children;
        }

        /**
         * Returns the source that this node corresponds to.
         * If the input token is trivial, then it returns an std::basic_string_view,
         * otherwise it returns a vector that is copied from the source.
         * @return the source that this match corresponds to.
         */
        auto source() const noexcept {
            if constexpr (std::is_trivial_v<input_token_type>) {
                return std::basic_string_view<input_token_type>(m_span.data(), m_span.size());
            }
            else {
                return std::vector<input_token_type>(m_span.begin(), m_span.end());
            }
        }

        /**
         * Used for debugging, prints the match and its children in the given output stream.
         * @param stream output stream.
         * @param get_id_name function to convert the id of the node to a string.
         * @param depth tree depth; affects the idententation.
         * @param tabSize characters per tab.
         */
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
        match_id_type m_id;
        input_span_type m_span;
        match_container_type m_children;
    };


} //namespace parserlib


#endif //PARSERLIB_MATCH_HPP
