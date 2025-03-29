#ifndef PARSERLIB_PARSE_CONTEXT_HPP
#define PARSERLIB_PARSE_CONTEXT_HPP


#include <cassert>
#include <vector>
#include <utility>
#include <cstddef>
#include <algorithm>
#include <map>
#include <cctype>
#include <type_traits>
#include <string_view>


namespace parserlib {


    template <class ParseContext> class rule;


    class case_sensitive_comparator {
    public:
        template <class T1, class T2>
        int operator ()(const T1& t1, const T2& t2) const noexcept {
            return t1 - t2;
        }
    };


    class case_insensitive_comparator {
    public:
        template <class T1, class T2>
        int operator ()(const T1& t1, const T2& t2) const noexcept {
            return std::tolower(t1) - std::tolower(t2);
        }
    };


    template <class Source, class MatchId, class ErrorId, class Comparator>
    class parse_context {
    public:
        // TYPES --------------------------------------------------------------

        using parse_context_type = parse_context<Source, MatchId, ErrorId, Comparator>;
        using source_type = Source;
        using value_type = typename Source::value_type;
        using match_id_type = MatchId;
        using error_id_type = ErrorId;
        using comparator_type = Comparator;
        using iterator_type = typename source_type::const_iterator;
        using this_type = parse_context<Source, MatchId, ErrorId, Comparator>;
        using rule_type = rule<this_type>;

        // CONSTRUCTOR --------------------------------------------------------

        parse_context(Source& source) noexcept
            : m_source(source)
            , m_parse_position(source.begin())
        {
        }

        // STATE --------------------------------------------------------------

        class state {
        public:
            state(const iterator_type& position, const std::size_t match_count) noexcept
                : m_position(position)
                , m_match_count(match_count)
            {
            }

            const iterator_type& position() const noexcept {
                return m_position;
            }

            std::size_t match_count() const noexcept {
                return m_match_count;
            }

        private:
            iterator_type m_position;
            std::size_t m_match_count;
        };

        const Source& source() const noexcept {
            return m_source;
        }

        const iterator_type& parse_position() const noexcept {
            return m_parse_position;
        }

        auto end_parse_position() const noexcept {
            return m_source.end();
        }

        bool is_valid_parse_position() const noexcept {
            return m_parse_position < m_source.end();
        }

        bool is_end_parse_position() const noexcept {
            return m_parse_position == m_source.end();
        }

        void set_parse_position(const iterator_type& position) noexcept {
            assert(position <= m_source.end());
            m_parse_position = position;
        }

        void increment_parse_position() noexcept {
            assert(m_parse_position < m_source.end());
            ++m_parse_position;
        }

        void increment_parse_position(std::size_t count) noexcept {
            assert(std::next(m_parse_position, count) <= m_source.end());
            m_parse_position = std::next(m_parse_position, count);
        }

        class state state() const noexcept {
            return { m_parse_position, m_matches.size() };
        }

        void set_state(const class state& state) noexcept {
            assert(state.position() <= m_source.end());
            m_parse_position = state.position();
            m_matches.resize(state.match_count());
        }

        template <class It>
        static auto source(const It& begin, const It& end) noexcept {
            if constexpr (std::is_integral_v<value_type>) {
                return std::string_view(&*begin, std::distance(begin, end));
            }
            else {
                return std::vector(begin, end);
            }
        }

        // COMPARISONS --------------------------------------------------------

        template <class T1, class T2>
        static int compare(const T1& t1, const T2& t2) noexcept {
            return comparator_type()(t1, t2);
        }

        // MATCHES ------------------------------------------------------------

        class match {
        public:
            match() {
            }

            match(const match_id_type& id, const iterator_type& begin, const iterator_type& end, std::vector<match>&& children) noexcept
                : m_id(id)
                , m_begin(begin)
                , m_end(end)
                , m_children(std::move(children))
            {
                assert(m_begin < m_end);
            }

            const match_id_type& id() const noexcept {
                return m_id;
            }

            const iterator_type& begin() const noexcept {
                return m_begin;
            }

            const iterator_type& end() const noexcept {
                return m_end;
            }

            const std::vector<match>& children() const noexcept {
                return m_children;
            }

            auto source() const noexcept {
                return parse_context_type::source(m_begin, m_end);
            }

        private:
            match_id_type m_id;
            iterator_type m_begin;
            iterator_type m_end;
            std::vector<match> m_children;
        };

        using match_container_type = std::vector<match>;

        const match_container_type& matches() const noexcept {
            return m_matches;
        }

        void add_match(const match_id_type& id, const iterator_type& begin, const iterator_type& end, const std::size_t match_count) noexcept {
            assert(match_count <= m_matches.size());
            const auto matches_begin = std::next(m_matches.begin(), match_count);
            const auto matches_end = m_matches.end();
            match new_match{ id, begin, end, { matches_begin, matches_end } };
            m_matches.resize(match_count);
            m_matches.push_back(std::move(new_match));
        }

        // ERRORS -------------------------------------------------------------

        class error {
        public:
            error(const error_id_type& id, const iterator_type& begin, const iterator_type& end) noexcept
                : m_id(id)
                , m_begin(begin)
                , m_end(end)
            {
                assert(m_begin <= m_end);
            }

            const error_id_type& id() const noexcept {
                return m_id;
            }

            const iterator_type& begin() const noexcept {
                return m_begin;
            }

            const iterator_type& end() const noexcept {
                return m_end;
            }

            auto source() const noexcept {
                return parse_context_type::source(m_begin, m_end);
            }

        private:
            error_id_type m_id;
            iterator_type m_begin;
            iterator_type m_end;
        };

        using error_container_type = std::vector<error>;

        const error_container_type& errors() const noexcept {
            return m_errors;
        }

        void add_error(const error_id_type& id, const iterator_type& begin, const iterator_type& end) noexcept {
            m_errors.emplace_back(id, begin, end);
        }

        // RULES ------------------------------------------------------------------

        bool is_left_recursive_rule(rule_type& rule) const noexcept {
            const auto it = m_rule_parse_positions.find(rule.pointer());
            return it != m_rule_parse_positions.end() && !it->second.empty() ? it->second.back() == m_parse_position : false;
        }

        void push_rule_parse_position(rule_type& rule) noexcept {
            m_rule_parse_positions[rule.pointer()].push_back(m_parse_position);
        }

        void pop_rule_parse_position(rule_type& rule) noexcept {
            assert(!m_rule_parse_positions[rule.pointer()].empty());
            m_rule_parse_positions[rule.pointer()].pop_back();
        }

    private:
        using rule_parse_position_map_type = std::map<rule_type*, std::vector<iterator_type>>;

        Source& m_source;
        iterator_type m_parse_position;
        match_container_type m_matches;
        error_container_type m_errors;
        rule_parse_position_map_type m_rule_parse_positions;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_HPP
