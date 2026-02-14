#ifndef PARSERLIB_PARSE_CONTEXT_HPP
#define PARSERLIB_PARSE_CONTEXT_HPP


#include <cassert>
#include <string>
#include <map>
#include "source_range.hpp"
#include "symbol_comparator.hpp"
#include "parse_node.hpp"


namespace parserlib {


    /**
     * The left recursion status for a rule.
     */ 
    enum class left_recursion_status {
        no_left_recursion,
        reject_left_recursion,
        accept_left_recursion
    };


    template <class ParseContext> class parser;


    /**
     * Class with parse state.
     * @param Iterator iterator to use.
     * @param MatchId type of match id.
     * @param ErrorId type of error id.
     */ 
    template <class Iterator = std::string::const_iterator, class MatchId = int, class ErrorId = int, class SymbolComparator = default_symbol_comparator>
    class parse_context {
    public:
        /** The iterator type. */
        using iterator_type = Iterator;

        /** the match id type. */
        using match_id_type = MatchId;

        /** the match type. */
        using match_type = match<MatchId, Iterator>;

        /** the match container type. */
        using match_container_type = std::vector<match_type>;

        /** The symbol comparator type. */ 
        using symbol_comparator_type = SymbolComparator;

        /**
         * Interface for parse nodes.
         * It is used to allow a parse context to keep left recursion state per node.
         */ 
        class parse_node : public parserlib::parse_node {
        };

        /**
         * The match start state.
         */ 
        class match_start_state {
        private:
            Iterator m_iterator;
            size_t m_match_count;

            match_start_state(const Iterator& iterator, size_t match_count = 0)
                : m_iterator(iterator)
                , m_match_count(match_count)
            {
            }

            friend class parse_context;
        };

        /**
         * The parse context state.
         */ 
        class state {
        public:
            /**
             * The constructor.
             * @param begin the initial iterator.
             * @param end the end iterator.
             */ 
            state(const Iterator& begin, const Iterator& end) 
                : m_iterator(begin)
                , m_match_count(0)
                , m_end(end)
                , m_match_start_state(begin)
            {
            }

            /**
             * Returns the iterator.
             * @return the iterator.
             */ 
            const Iterator& get_iterator() const {
                return m_iterator;
            }

        private:
            Iterator m_iterator;
            size_t m_match_count;
            Iterator m_end;
            match_start_state m_match_start_state;

            friend class parse_context;
        };

        /**
         * State of left recursion for a parse node.
         */ 
        class left_recursion_state {
        public:
            /**
             * The default constructor.
             * @param iterator iterator.
             * @param status status.
             */ 
            left_recursion_state(const Iterator& iterator = {}, left_recursion_status status = {})
                : m_iterator(iterator), m_status(status)
            {
            }

            /**
             * Returns the last position a node was parsed at.
             * @return the last position a node was parsed at.
             */ 
            const Iterator& get_iterator() const {
                return m_iterator;
            }

            /**
             * Returns the current left recursion status for a node.
             * @return the current left recursion status for a node.
             */ 
            left_recursion_status get_status() const {
                return m_status;
            }

        private:
            Iterator m_iterator;
            left_recursion_status m_status;
        };

        /**
         * Constructor from source range.
         * @param begin the begin iterator.
         * @param end the end iterator.
         */ 
        parse_context(const Iterator& begin, const Iterator& end)
            : m_state(begin, end)
            , m_end(end)
        {
        }

        /**
         * Constructor from a container.
         * @param container the source container.
         */ 
        template <class Container>
        parse_context(const Container& container)
            : parse_context(container.begin(), container.end())
        {
        }

        /**
         * Returns the current state.
         * @return the current state.
         */ 
        const state& get_state() const {
            return m_state;
        }

        /**
         * Sets the current state.
         * @param s the state to set.
         */ 
        void set_state(const state& s) {
            m_state = s;
            m_matches.resize(m_state.m_match_count);
        }

        /**
         * Compares two symbols using the supplied symbol comparator.
         * @param left the left symbol.
         * @param right the right symbol.
         * @return their difference.
         */ 
        template <class L, class R>
        static int compare(const L& left, const R& right) {
            return SymbolComparator::compare(left, right);
        }

        /**
         * Returns the current end iterator.
         * @return the current end iterator.
         */ 
        const Iterator& get_end_iterator() const {
            return m_state.m_end;
        }

        /**
         * Returns the current iterator.
         * @return the current iterator.
         */ 
        const Iterator& get_iterator() const {
            return m_state.m_iterator;
        }

        /**
         * Checks if the end position is not reached.
         * @return true if the end position is not reached, false otherwise.
         */ 
        bool is_valid_iterator() const {
            return m_state.m_iterator != m_state.m_end;
        }

        /**
         * Checks if the end position is reached.
         * @return true if the end position is reached, false otherwise.
         */ 
        bool is_end_iterator() const {
            return m_state.m_iterator == m_state.m_end;
        }

        /**
         * Increments the iterator by one position.
         */ 
        void increment_iterator() {
            assert(is_valid_iterator());
            ++m_state.m_iterator;
            update_match_start_state();
        }

        /**
         * Increments the iterator by the specific count.
         * @param count the increment count.
         */ 
        void increment_iterator(size_t count) {
            assert(is_valid_iterator());
            m_state.m_iterator += count;
            update_match_start_state();
        }

        /**
         * Increments the line.
         * Valid only if the iterator supports it.
         */ 
        void increment_line() {
            m_state.m_iterator.increment_line();
        }

        /**
         * Returns the container of matches.
         * @return the container of matches.
         */ 
        const match_container_type& get_matches() const {
            return m_matches;
        }

        /**
         * Returns the current match start state.
         * @return the current match start state.
         */ 
        const match_start_state& get_match_start_state() const {
            return m_state.m_match_start_state;
        }

        /**
         * Sets the match start state.
         * @param state the match start state.
         */ 
        void set_match_start_state(const match_start_state& state) {
            m_state.m_match_start_state = state;
        }

        /**
         * Adds a match.
         * The range of the match is from the provided state up to the current state.
         * @param id id.
         * @param from_state the initial state to start the match from.
         */ 
        void add_match(const MatchId& id, const match_start_state& from_state) {
            match_container_type children(m_matches.begin() + from_state.m_match_count, m_matches.end());
            m_matches.resize(from_state.m_match_count);
            m_matches.push_back(match_type(id, from_state.m_iterator, m_state.m_iterator, std::move(children)));
            m_state.m_match_count = m_matches.size();
            m_state.m_match_start_state.m_match_count = m_state.m_match_count;
        }

        /**
         * Creates a parse context that can be used to parse the matches of this parse context.
         * @return a new parse context that can be used to parse the matches of this parse context.
         */ 
        template <class DerivedMatchId = int, class DerivedErrorId = int, class DerivedSymbolComparator = default_symbol_comparator>
        parse_context<typename match_container_type::const_iterator, DerivedMatchId, DerivedErrorId, DerivedSymbolComparator>
        derive_parse_context() const {
            return m_matches;
        }

        /**
         * Returns the left recursion state for a node at the given address,
         * and a flag that indicates if this state is the initial state for the node.
         * If the node does not have any state, then the a default state is set and returned for the node.
         * @param node pointer to node address to get the left recursion state of.
         * @return the left recursion state for the given node and the initial state flag.
         */ 
        std::pair<left_recursion_state, bool> get_or_create_left_recursion_state(const parse_node* node) {
            auto it = m_left_recursion_map.find(node);
            if (it != m_left_recursion_map.end()) {
                return std::make_pair(it->second, false);
            }
            auto [it1, ok] = m_left_recursion_map.insert(std::make_pair(node, left_recursion_state(m_state.m_iterator, left_recursion_status::no_left_recursion)));
            return std::make_pair(it1->second, true);
        }   

        /**
         * Sets the left recursion state for a node at the given address from a previous state.
         * @param node pointer to node address to get the left recursion state of.
         * @param state the state.
         */ 
        void set_left_recursion_state(const parse_node* node, const left_recursion_state& state) {
            m_left_recursion_map[node] = state;
        }

        /**
         * Sets the left recursion state for a node at the given address.
         * It uses the current iterator and the given status for the new state.
         * @param node pointer to node address to get the left recursion state of.
         * @param status the new status.
         */ 
        void set_new_left_recursion_state(const parse_node* node, left_recursion_status status) {
            m_left_recursion_map[node] = left_recursion_state(m_state.m_iterator, status);
        }

        /**
         * Enables terminal parsing.
         */ 
        void enable_terminal_parsing() {
            m_state.m_end = m_end;
        }

        /**
         * Disables terminal parsing.
         */ 
        void disable_terminal_parsing() {
            m_state.m_end = m_state.m_iterator;
        }

    private:
        using left_recursion_map = std::map<const parse_node*, left_recursion_state>;

        state m_state;
        const Iterator m_end;
        match_container_type m_matches;
        left_recursion_map m_left_recursion_map;

        void update_match_start_state() {
            m_state.m_match_start_state.m_iterator = m_state.m_iterator;
            m_state.m_match_start_state.m_match_count = m_state.m_match_count;
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_HPP
