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

        template <class Iterator, class MatchId, class ErrorId, class SymbolComparator>
        friend class parse_context;
    };


    template <class Iterator>
    class parse_context_state {
    public:
        using iterator_type = Iterator;

        parse_context_state(const Iterator& begin, const Iterator& end)
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

    private:
        parse_state<Iterator> m_parse_state;
        parse_state<Iterator> m_match_parse_state;
        Iterator m_end_iterator;
        size_t m_error_count;

        template <class Iterator, class MatchId, class ErrorId, class SymbolComparator>
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

        template <class Iterator, class MatchId, class ErrorId, class SymbolComparator>
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

        using match_type = match<Iterator, MatchId>;
        using match_container_type = std::vector<match_type>;

        using error_type = parserlib::error<Iterator, ErrorId>;
        using error_container_type = std::vector<error_type>;

        using parse_node_type = parse_node<parse_context>;
        using left_recursion_exception_type = left_recursion_exception<parse_context>;

        parse_context(const Iterator& begin, const Iterator& end)
            : m_state(begin, end)
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

        const parse_state_type& get_match_parse_state() const {
            return m_state.m_match_parse_state;
        }

        const Iterator& get_iterator() const {
            return m_state.m_parse_state.m_iterator;
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
            return *m_curr_error_container;
        }

        void add_error(const ErrorId& id, const Iterator& from_iterator) {
            m_curr_error_container->push_back(error_type(id, from_iterator, m_state.m_parse_state.m_iterator));
        }

        bool parse_branch(const parse_node_type* parse_node, size_t base_error_count, error_container_type& branch_errors) {
            error_container_type* prev_error_container = m_curr_error_container;

            try {
                m_curr_error_container = &branch_errors;
                const bool result = parse_node->parse(*this);
                m_curr_error_container = prev_error_container;

                if (branch_errors.empty()) {
                    if (result) {
                        prev_error_container->resize(base_error_count);
                    }
                }
                else {
                    if (prev_error_container->empty()) {
                        prev_error_container->insert(prev_error_container->end(), branch_errors.begin(), branch_errors.end());
                    }
                    else {
                        const error_type& new_error = branch_errors.back();
                        const error_type& old_error = prev_error_container->back();
                        const auto new_dist_to_end = std::distance(new_error.end(), m_end_iterator);
                        const auto old_dist_to_end = std::distance(old_error.end(), m_end_iterator);
                        if (new_dist_to_end < old_dist_to_end) {
                            prev_error_container->resize(base_error_count);
                            prev_error_container->insert(prev_error_container->end(), branch_errors.begin(), branch_errors.end());
                        }
                    }
                    branch_errors.clear();
                }

                return result;
            }
            catch (...) {
                m_curr_error_container = prev_error_container;
                throw;
            }
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
        Iterator m_end_iterator;
        match_container_type m_matches;
        error_container_type m_errors;
        error_container_type* m_curr_error_container{&m_errors};
        left_recursion_state_map m_left_recursion_states;

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
