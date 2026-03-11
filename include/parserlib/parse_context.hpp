#ifndef PARSERLIB_PARSE_CONTEXT_HPP
#define PARSERLIB_PARSE_CONTEXT_HPP


#include <string>
#include <map>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include "match.hpp"
#include "error.hpp"
#include "parse_node.hpp"
#include "text_iterator.hpp"


namespace parserlib {


    template <class Iterator, class MatchId, class ErrorId, class SymbolComparator>
    class parse_context;


    template <class Iterator>
    class parse_state {
    public:
        using iterator_type = Iterator;

        parse_state(const Iterator& iterator, size_t match_count = 0)
            : m_iterator(iterator)
            , m_match_count(match_count)
        {
        }

        const Iterator& get_iterator() const {
            return m_iterator;
        }

        size_t get_match_count() const {
            return m_match_count;
        }

    private:
        Iterator m_iterator;
        size_t m_match_count;

        template <class Iterator1, class MatchId, class ErrorId, class SymbolComparator>
        friend class parse_context;
    };


    template <class Iterator>
    class parse_context_state {
    public:
        using iterator_type = Iterator;

        parse_context_state(const Iterator& begin = {}, const Iterator& end = {})
            : m_parse_state(begin)
            , m_match_parse_state(begin)
            , m_end_iterator(end)
            , m_error_count(0)
        {
        }

        const parse_state<Iterator>& get_parse_state() const {
            return m_parse_state;
        }

        const parse_state<Iterator>& get_match_parse_state() const {
            return m_match_parse_state;
        }

        const Iterator& get_end_iterator() const {
            return m_end_iterator;
        }

        size_t get_error_count() const {
            return m_error_count;
        }

        const Iterator& get_iterator() const {
            return m_parse_state.get_iterator();
        }

    private:
        parse_state<Iterator> m_parse_state;
        parse_state<Iterator> m_match_parse_state;
        Iterator m_end_iterator;
        size_t m_error_count;

        template <class Iterator1, class MatchId, class ErrorId, class SymbolComparator>
        friend class parse_context;
    };


    template <class Iterator, class MatchId, class ErrorId>
    class parse_context_memoized_state {
    public:
        parse_context_memoized_state() {
        }

        const parse_context_state<Iterator>& get_state() const {
            return m_state;
        }

        const std::vector<match<Iterator, MatchId>>& get_matches() const {
            return m_matches;
        }

        const std::vector<error<Iterator, ErrorId>>& get_errors() const {
            return m_errors;
        }

    private:
        parse_context_state<Iterator> m_state;
        size_t m_base_match_count;
        size_t m_base_error_count;
        std::vector<match<Iterator, MatchId>> m_matches;
        std::vector<error<Iterator, ErrorId>> m_errors;

        template <class Iterator1, class MatchId1, class ErrorId1, class SymbolComparator>
        friend class parse_context;
    };


    enum class left_recursion_status {
        none,
        reject,
        accept
    };


    template <class Iterator>
    class left_recursion_state {
    public:
        using iterator_type = Iterator;

        left_recursion_state(const Iterator& iterator = {}, left_recursion_status status = {})
            : m_iterator(iterator)
            , m_status(status)
        {
        }

        const Iterator& get_iterator() const {
            return m_iterator;
        }

        left_recursion_status get_status() const {
            return m_status;
        }

    private:
        Iterator m_iterator;
        left_recursion_status m_status;

        template <class Iterator1, class MatchId, class ErrorId, class SymbolComparator>
        friend class parse_context;
    };


    template <class ParseContext>
    class left_recursion_exception {
    public:
        using parse_node_type = parse_node<ParseContext>;

        left_recursion_exception(const parse_node_type* parse_node)
            : m_parse_node(parse_node)
        {
        }

        const parse_node_type* get_parse_node() const {
            return m_parse_node;
        }

    private:
        const parse_node_type* m_parse_node;
    };


    class default_symbol_comparator {
    public:
        template <class L, class R>
        static int compare(const L& l, const R& r) {
            return static_cast<int>(l) - static_cast<int>(r);
        }
    };


    class case_insensitive_symbol_comparator {
    public:
        template <class L, class R>
        static int compare(const L& l, const R& r) {
            return std::tolower(static_cast<int>(l)) - std::tolower(static_cast<int>(r));
        }
    };


    template <class Iterator>
    void increment_line(Iterator& it) {
    }


    template <class Iterator = std::string::const_iterator, class MatchId = int, class ErrorId = int, class SymbolComparator = default_symbol_comparator>
    class parse_context {
    public:
        using iterator_type = Iterator;
        using match_id_type = MatchId;
        using error_id_type = ErrorId;
        using symbol_comparator_type = SymbolComparator;

        using parse_state_type = parse_state<Iterator>;
        using parse_context_state_type = parse_context_state<Iterator>;

        using parse_context_memoized_state_type = parse_context_memoized_state<Iterator, MatchId, ErrorId>;

        using match_type = match<Iterator, MatchId>;
        using match_container_type = std::vector<match_type>;

        using error_type = parserlib::error<Iterator, ErrorId>;
        using error_container_type = std::vector<error_type>;

        using parse_node_type = parse_node<parse_context>;
        using left_recursion_exception_type = left_recursion_exception<parse_context>;

        template <class DerivedMatchId, class DerivedErrorId, class DerivedSymbolComparator = default_symbol_comparator>
        struct derived_parse_context {
            using type = parserlib::parse_context<typename match_container_type::const_iterator, DerivedMatchId, DerivedErrorId, DerivedSymbolComparator>;
        };

        template <class DerivedMatchId, class DerivedErrorId, class DerivedSymbolComparator = default_symbol_comparator>
        using derived_parse_context_type = typename derived_parse_context<DerivedMatchId, DerivedErrorId, DerivedSymbolComparator>::type;

        parse_context(const Iterator& begin, const Iterator& end)
            : m_state(begin, end)
            , m_begin_iterator(begin)
            , m_end_iterator(end)
        {
        }

        template <class Container>
        parse_context(const Container& container)
            : parse_context(container.begin(), container.end())
        {
        }

        const parse_context_state_type& get_state() const {
            return m_state;
        }

        void set_state(const parse_context_state_type& state) {
            m_state = state;
            m_matches.resize(state.m_parse_state.m_match_count);
            m_errors.resize(state.m_error_count);
        }

        parse_context_memoized_state_type get_memoized_state(const parse_context_state_type& base_state) const {
            parse_context_memoized_state_type result;
            result.m_state = m_state;
            result.m_base_match_count = base_state.m_match_parse_state.m_match_count;
            result.m_base_error_count = base_state.m_error_count;
            result.m_matches.insert(result.m_matches.end(), m_matches.begin() + base_state.m_match_parse_state.m_match_count, m_matches.end());
            result.m_errors.insert(result.m_errors.end(), m_errors.begin() + base_state.m_error_count, m_errors.end());
            return result;
        }

        void set_memoized_state(const parse_context_memoized_state_type& mem_state) {
            m_state = mem_state.m_state;
            m_matches.resize(mem_state.m_base_match_count);
            m_matches.insert(m_matches.end(), mem_state.m_matches.begin(), mem_state.m_matches.end());
            m_errors.resize(mem_state.m_base_error_count);
            m_errors.insert(m_errors.end(), mem_state.m_errors.begin(), mem_state.m_errors.end());
        }

        const parse_state_type& get_match_parse_state() const {
            return m_state.m_match_parse_state;
        }

        const Iterator& get_iterator() const {
            return m_state.m_parse_state.m_iterator;
        }

        const Iterator& get_begin_iterator() const {
            return m_state.m_begin_iterator;
        }

        const Iterator& get_end_iterator() const {
            return m_state.m_end_iterator;
        }

        bool is_valid_iterator() const {
            return m_state.m_parse_state.m_iterator != m_state.m_end_iterator;
        }

        void increment_iterator() {
            ++m_state.m_parse_state.m_iterator;
            m_state.m_match_parse_state = m_state.m_parse_state;
        }

        void increment_iterator(size_t count) {
            m_state.m_parse_state.m_iterator += count;
            m_state.m_match_parse_state = m_state.m_parse_state;
        }

        void increment_line() {
            parserlib::increment_line(m_state.m_parse_state.m_iterator);
            m_state.m_match_parse_state = m_state.m_parse_state;
        }

        template <class L, class R>
        static int compare(const L& l, const R& r) {
            return SymbolComparator::compare(l, r);
        }

        const match_container_type& get_matches() const {
            return m_matches;
        }

        void add_match(const MatchId& id, const parse_state_type& from_state) {
            match_container_type children(m_matches.begin() + from_state.m_match_count, m_matches.end());
            m_matches.resize(from_state.m_match_count);
            m_matches.push_back(match_type(id, from_state.m_iterator, m_state.m_parse_state.m_iterator, std::move(children)));
            m_state.m_parse_state.m_match_count = m_matches.size();
            m_state.m_match_parse_state.m_match_count = m_matches.size();
        }

        const error_container_type& get_errors() const {
            return m_errors;
        }

        void add_error(const ErrorId& id, const Iterator& from_iterator) {
            m_errors.push_back(error_type(id, from_iterator, m_state.m_parse_state.m_iterator));
            m_state.m_error_count = m_errors.size();
        }

        bool parse_left_recursion(const parse_node_type* parse_node) {
            auto it = m_left_recursion_states.find(parse_node);

            if (it == m_left_recursion_states.end()) {
                auto [it, ok] = m_left_recursion_states.insert(std::make_pair(parse_node, left_recursion_state_type(m_state.m_parse_state.m_iterator, left_recursion_status::none)));

                try {
                    return parse_node->parse(*this);
                }

                catch (const left_recursion_exception_type& ex) {
                    if (ex.get_parse_node() == parse_node) {
                        return handle_left_recursion(parse_node, it->second);
                    }
                    throw ex;
                }
            }

            if (m_state.m_parse_state.m_iterator != it->second.m_iterator) {
                const left_recursion_state_type prev_left_recursion_state = it->second;
                it->second = left_recursion_state_type(m_state.m_parse_state.m_iterator, left_recursion_status::none);

                try {
                    const bool result = parse_node->parse(*this);
                    it->second = prev_left_recursion_state;
                    return result;
                }

                catch (const left_recursion_exception_type& ex) {
                    it->second = prev_left_recursion_state;
                    if (ex.get_parse_node() == parse_node) {
                        return handle_left_recursion(parse_node, it->second);
                    }
                    throw ex;
                }

                catch (...) {
                    it->second = prev_left_recursion_state;
                    throw;
                }
            }

            switch (it->second.m_status) {
                case left_recursion_status::none:
                    throw left_recursion_exception_type(parse_node);

                case left_recursion_status::reject:
                    return false;

                case left_recursion_status::accept:
                    unlock_iterator();
                    return true;
            }

            throw std::runtime_error("invalid state");
        }

        template <class DerivedMatchId = int, class DerivedErrorId = int, class DerivedSymbolComparator = default_symbol_comparator>
        auto derive_parse_context() const {
            return parse_context<typename match_container_type::const_iterator, DerivedMatchId, DerivedErrorId, DerivedSymbolComparator>(m_matches);
        }

    private:
        using left_recursion_state_type = left_recursion_state<Iterator>;
        using left_recursion_state_map = std::map<const parse_node_type*, left_recursion_state_type>;

        parse_context_state_type m_state;
        match_container_type m_matches;
        error_container_type m_errors;
        left_recursion_state_map m_left_recursion_states;
        const Iterator m_begin_iterator;
        const Iterator m_end_iterator;

        void lock_iterator() {
            m_state.m_end_iterator = m_state.m_parse_state.m_iterator;
        }

        void unlock_iterator() {
            m_state.m_end_iterator = m_end_iterator;
        }

        void set_match_parse_state(const parse_state_type& state) {
            m_state.m_match_parse_state = state;
        }

        bool handle_left_recursion(const parse_node_type* parse_node, left_recursion_state_type& left_recursion_state) {
            parse_state_type base_match_parse_state = get_match_parse_state();
            const left_recursion_state_type prev_left_recursion_state = left_recursion_state;
            left_recursion_state = left_recursion_state_type(m_state.m_parse_state.m_iterator, left_recursion_status::reject);

            try {
                const bool result = parse_node->parse(*this);
                if (!result) {
                    set_match_parse_state(base_match_parse_state);
                    left_recursion_state = prev_left_recursion_state;
                    return false;
                }
            }
            catch (...) {
                set_match_parse_state(base_match_parse_state);
                left_recursion_state = prev_left_recursion_state;
                throw;
            }

            for (;;) {
                set_match_parse_state(base_match_parse_state);
                lock_iterator();
                left_recursion_state = left_recursion_state_type(m_state.m_parse_state.m_iterator, left_recursion_status::accept);

                try {
                    const bool result = parse_node->parse(*this);
                    if (!result) {
                        set_match_parse_state(base_match_parse_state);
                        unlock_iterator();
                        left_recursion_state = prev_left_recursion_state;
                        break;
                    }
                }
                catch (...) {
                    set_match_parse_state(base_match_parse_state);
                    unlock_iterator();
                    left_recursion_state = prev_left_recursion_state;
                    throw;
                }
            }

            return true;
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_HPP
