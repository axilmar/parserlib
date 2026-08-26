#ifndef PARSERLIB_HPP
#define PARSERLIB_HPP


#include <algorithm>
#include <cassert>
#include <cctype>
#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>


namespace parserlib {


    class default_symbol_comparator {
    public:
        template <class L, class R>
        static int compare(const L& left, const R& right) noexcept {
            return static_cast<int>(left) - static_cast<int>(right);
        }
    };


    class case_insensitive_symbol_comparator {
    public:
        template <class L, class R>
        static int compare(const L& left, const R& right) noexcept {
            return std::tolower(static_cast<int>(left)) - std::tolower(static_cast<int>(right));
        }
    };


    template <class Iterator>
    void increment_iterator_line(Iterator& iterator) {
    }


    template <typename T, typename = std::void_t<>>
    struct is_valid_basic_string : std::false_type {
    };


    template <typename T>
    struct is_valid_basic_string<T, std::void_t<typename std::basic_string<T>>> : std::true_type {
    };


    template <class Iterator = std::string::const_iterator>
    class text_position_iterator : public Iterator {
    public:
        text_position_iterator(const Iterator& iterator = {}) : Iterator(iterator) {
        }

        text_position_iterator& operator ++() {
            Iterator::operator ++();
            ++m_column;
            return *this;
        }

        text_position_iterator& operator ++(int) {
            text_position_iterator result = *this;
            Iterator::operator ++();
            ++m_column;
            return result;
        }

        size_t get_line() const noexcept {
            return m_line;
        }

        size_t get_column() const noexcept {
            return m_column;
        }

        void increment_line() noexcept {
            ++m_line;
            m_column = 1;
        }

    private:
        size_t m_line{ 1 };
        size_t m_column{ 1 };
    };


    template <class Iterator>
    void increment_iterator_line(text_position_iterator<Iterator>& iterator) {
        iterator.increment_line();
    }


    template <class Iterator = typename std::string::const_iterator, class MatchId = int, class ErrorId = int, class SymbolComparator = default_symbol_comparator>
    class parser {
    public:
        using iterator_type = Iterator;
        using match_id_type = MatchId;
        using error_id_type = ErrorId;
        using symbol_comparator_type = SymbolComparator;

        class source_instance {
        public:
            source_instance(const Iterator& begin, const Iterator& end)
                : m_begin(begin)
                , m_end(end)
            {
            }

            auto get_source() const {
                using value_type = std::decay_t<typename Iterator::value_type>;

                if constexpr (is_valid_basic_string<value_type>::value) {
                    return std::basic_string<value_type>{ m_begin, m_end };

                }
                else {
                    return std::vector<value_type>{ m_begin, m_end };
                }
            }

            const Iterator& begin() const noexcept {
                return m_begin;
            }

            const Iterator& end() const noexcept {
                return m_end;
            }

        private:
            Iterator m_begin;
            Iterator m_end;
        };

        class match_instance : public source_instance {
        public:
            match_instance(const Iterator& begin = {}, const Iterator& end = {}, MatchId id = {}, std::vector<match_instance>&& children = {})
                : source_instance(begin, end)
                , m_id(id)
                , m_children(std::move(children))
            {
            }

            MatchId get_id() const noexcept {
                return m_id;
            }

            const std::vector<match_instance>& get_matches() const noexcept {
                return m_children;
            }

            const std::vector<match_instance>& get_children() const noexcept {
                return m_children;
            }

        private:
            MatchId m_id;
            std::vector<match_instance> m_children;
        };

        class error_instance : public source_instance {
        public:
            error_instance(const Iterator& begin = {}, const Iterator& end = {}, ErrorId id = {})
                : source_instance(begin, end)
                , m_id(id)
            {
            }

            ErrorId get_id() const noexcept {
                return m_id;
            }

        private:
            ErrorId m_id;
        };

        class parse_context {
        public:
            struct state {
            public:

            private:
                Iterator m_iterator;
                size_t m_match_position_count;
                size_t m_match_count;

                state(const Iterator& it, size_t match_position_count, size_t match_count)
                    : m_iterator(it)
                    , m_match_position_count(match_position_count)
                    , m_match_count(match_count)
                {
                }

                friend class parse_context;
            };

            parse_context(const Iterator& begin, const Iterator& end)
                : m_iterator(begin)
                , m_end_iterator(end)
            {
            }

            template <class Container>
            parse_context(const Container& container)
                : parse_context(container.begin(), container.end()) {
            }

            state get_state() const noexcept {
                return { m_iterator, m_match_positions.size(), m_matches.size() };
            }

            void set_state(const state& st) {
                m_iterator = st.m_iterator;
                m_match_positions.resize(st.m_match_position_count);
                m_matches.resize(st.m_match_count);
            }

            const Iterator& get_iterator() const noexcept {
                return m_iterator;
            }

            void set_iterator(const Iterator& iterator) {
                m_iterator = iterator;
            }

            void increment_iterator() {
                ++m_iterator;
            }

            void increment_line() {
                increment_iterator_line(m_iterator);
            }

            const Iterator& get_end_iterator() const noexcept {
                return m_end_iterator;
            }

            bool is_valid_iterator() const noexcept {
                return m_iterator != m_end_iterator;
            }

            bool is_end_iterator() const noexcept {
                return m_iterator == m_end_iterator;
            }

            const std::vector<match_instance>& get_matches() const noexcept {
                return m_matches;
            }

            void push_match_position() {
                m_match_positions.push_back(match_position{ m_iterator, m_matches.size() });
            }

            void pop_match_position() {
                assert(m_match_positions.size() > 0);
                m_match_positions.pop_back();
            }

            void add_match(MatchId id) {
                assert(m_match_positions.size() > 0);
                const match_position& mp = m_match_positions.back();
                std::vector<match_instance> children{std::make_move_iterator(m_matches.begin() + mp.match_count), std::make_move_iterator(m_matches.end())};
                m_matches.resize(mp.match_count);
                m_matches.push_back(match_instance{ mp.iterator, m_iterator, id, std::move(children) });
            }

            const std::vector<error_instance>& get_errors() const noexcept {
                return m_errors;
            }

            void add_error(const Iterator& begin, const Iterator& end, ErrorId id) {
                m_errors.push_back(error_instance{ begin, end, id });
            }

            template <class L, class R>
            static int compare(const L& left, const R& right) noexcept {
                return SymbolComparator::compare(left, right);
            }

        private:
            struct match_position {
                Iterator iterator;
                size_t match_count;
            };

            Iterator m_iterator;
            const Iterator m_end_iterator;
            std::vector<match_position> m_match_positions;
            std::vector<match_instance> m_matches;
            std::vector<error_instance> m_errors;
        };

        class parse_node {
        public:
            virtual ~parse_node() {}
            virtual bool parse(parse_context& pc) const = 0;
        };

        template <class Symbol>
        class symbol_parse_node : public parse_node {
        public:
            symbol_parse_node(Symbol symbol) : m_symbol(symbol) {
            }

            Symbol get_symbol() const noexcept {
                return m_symbol;
            }

            bool parse(parse_context& pc) const override {
                if (pc.is_valid_iterator()) {
                    const auto& source_symbol = *pc.get_iterator();
                    if (parse_context::compare(source_symbol, m_symbol) == 0) {
                        pc.increment_iterator();
                        return true;
                    }
                }
                return false;
            }

        private:
            Symbol m_symbol;
        };

        template <class Symbol>
        class string_parse_node : public parse_node {
        public:
            string_parse_node(std::basic_string<Symbol> string) : m_string(string) {
            }

            const std::basic_string<Symbol>& get_string() const noexcept {
                return m_string;
            }

            bool parse(parse_context& pc) const override {
                auto string_it = m_string.begin();
                auto source_it = pc.get_iterator();
                for (;;) {
                    if (string_it == m_string.end()) {
                        pc.set_iterator(source_it);
                        return true;
                    }
                    if (source_it == pc.get_end_iterator()) {
                        break;
                    }
                    const auto& string_symbol = *string_it;
                    const auto& source_symbol = *source_it;
                    if (parse_context::compare(source_symbol, string_symbol) != 0) {
                        break;
                    }
                    ++string_it;
                    ++source_it;
                }
                return false;
            }

        private:
            std::basic_string<Symbol> m_string;
        };

        template <class Symbol>
        class set_parse_node : public parse_node {
        public:
            set_parse_node(std::vector<Symbol> set) : m_set(set) {
                std::sort(m_set.begin(), m_set.end());
            }

            const std::vector<Symbol>& get_set() const noexcept {
                return m_set;
            }

            bool parse(parse_context& pc) const override {
                if (pc.is_valid_iterator()) {
                    const auto& source_symbol = *pc.get_iterator();
                    auto it = std::upper_bound(m_set.begin(), m_set.end(), source_symbol, [](const auto& l, const auto& r) { return parse_context::compare(l, r) < 0; });
                    if (it != m_set.begin()) {
                        --it;
                        const auto& set_symbol = *it;
                        if (parse_context::compare(source_symbol, set_symbol) == 0) {
                            pc.increment_iterator();
                            return true;
                        }
                    }
                }
                return false;
            }

        private:
            std::vector<Symbol> m_set;
        };

        template <class Symbol>
        class range_parse_node : public parse_node {
        public:
            range_parse_node(Symbol min, Symbol max) : m_min(min), m_max(max) {
                assert(min <= max);
            }

            Symbol get_min() const noexcept {
                return m_min;
            }

            Symbol get_max() const noexcept {
                return m_max;
            }

            bool parse(parse_context& pc) const override {
                if (pc.is_valid_iterator()) {
                    const auto& source_symbol = *pc.get_iterator();
                    if (parse_context::compare(source_symbol, m_min) >= 0 && parse_context::compare(source_symbol, m_max) <= 0) {
                        pc.increment_iterator();
                        return true;
                    }
                }
                return false;
            }

        private:
            Symbol m_min, m_max;
        };

        using parse_node_ptr_type = std::shared_ptr<parse_node>;

        class loop_0_parse_node : public parse_node {
        public:
            loop_0_parse_node(const parse_node_ptr_type& child) : m_child(child) {
            }

            const parse_node_ptr_type& get_child() const noexcept {
                return m_child;
            }

            bool parse(parse_context& pc) const override {
                while (m_child->parse(pc) && pc.is_valid_iterator()) {
                }
                return true;
            }

        private:
            parse_node_ptr_type m_child;
        };

        class loop_1_parse_node : public parse_node {
        public:
            loop_1_parse_node(const parse_node_ptr_type& child) : m_child(child) {
            }

            const parse_node_ptr_type& get_child() const noexcept {
                return m_child;
            }

            bool parse(parse_context& pc) const override {
                if (m_child->parse(pc)) {
                    while (m_child->parse(pc) && pc.is_valid_iterator()) {
                    }
                    return true;
                }
                return false;
            }

        private:
            parse_node_ptr_type m_child;
        };

        class loop_n_parse_node : public parse_node {
        public:
            loop_n_parse_node(const parse_node_ptr_type& child, size_t count) : m_child(child), m_count(count) {
            }

            const parse_node_ptr_type& get_child() const noexcept {
                return m_child;
            }

            bool parse(parse_context& pc) const override {
                const parse_context::state prev_state = pc.get_state();
                for (size_t count = 0; count < m_count; ++count) {
                    if (!m_child->parse(pc)) {
                        pc.set_state(prev_state);
                        return false;
                    }
                }
                return true;
            }

        private:
            parse_node_ptr_type m_child;
            size_t m_count;
        };

        class optional_parse_node : public parse_node {
        public:
            optional_parse_node(const parse_node_ptr_type& child) : m_child(child) {
            }

            const parse_node_ptr_type& get_child() const noexcept {
                return m_child;
            }

            bool parse(parse_context& pc) const override {
                m_child->parse(pc);
                return true;
            }

        private:
            parse_node_ptr_type m_child;
        };

        class logical_and_parse_node : public parse_node {
        public:
            logical_and_parse_node(const parse_node_ptr_type& child) : m_child(child) {
            }

            const parse_node_ptr_type& get_child() const noexcept {
                return m_child;
            }

            bool parse(parse_context& pc) const override {
                const parse_context::state prev_state = pc.get_state();
                const bool result = m_child->parse(pc);
                pc.set_state(prev_state);
                return result;
            }

        private:
            parse_node_ptr_type m_child;
        };

        class logical_not_parse_node : public parse_node {
        public:
            logical_not_parse_node(const parse_node_ptr_type& child) : m_child(child) {
            }

            const parse_node_ptr_type& get_child() const noexcept {
                return m_child;
            }

            bool parse(parse_context& pc) const override {
                const parse_context::state prev_state = pc.get_state();
                const bool result = !m_child->parse(pc);
                pc.set_state(prev_state);
                return result;
            }

        private:
            parse_node_ptr_type m_child;
        };

        class sequence_parse_node : public parse_node {
        public:
            sequence_parse_node(std::vector<parse_node_ptr_type>&& children) : m_children(std::move(children)) {
            }

            const std::vector<parse_node_ptr_type>& get_children() const noexcept {
                return m_children;
            }

            bool parse(parse_context& pc) const override {
                const parse_context::state prev_state = pc.get_state();
                for (const parse_node_ptr_type& child : m_children) {
                    if (!child->parse(pc)) {
                        pc.set_state(prev_state);
                        return false;
                    }
                }
                return true;
            }

        private:
            std::vector<parse_node_ptr_type> m_children;
        };

        class choice_parse_node : public parse_node {
        public:
            choice_parse_node(std::vector<parse_node_ptr_type>&& children) : m_children(std::move(children)) {
            }

            const std::vector<parse_node_ptr_type>& get_children() const noexcept {
                return m_children;
            }

            bool parse(parse_context& pc) const override {
                for (const parse_node_ptr_type& child : m_children) {
                    const parse_context::state prev_state = pc.get_state();
                    if (child->parse(pc)) {
                        return true;
                    }
                    pc.set_state(prev_state);
                }
                return false;
            }

        private:
            std::vector<parse_node_ptr_type> m_children;
        };

        class match_parse_node : public parse_node {
        public:
            match_parse_node(const parse_node_ptr_type& child, MatchId id)
                : m_child(child)
                , m_id(id)
            {
            }

            const parse_node_ptr_type& get_child() const noexcept {
                return m_child;
            }

            MatchId get_id() const noexcept {
                return m_id;
            }

            bool parse(parse_context& pc) const override {
                pc.push_match_position();
                if (m_child->parse(pc)) {
                    pc.add_match(m_id);
                    pc.pop_match_position();
                    return true;
                }
                pc.pop_match_position();
                return false;
            }

        private:
            parse_node_ptr_type m_child;
            MatchId m_id;
        };


        class begin_match_parse_node : public parse_node {
        public:
            bool parse(parse_context& pc) const override {
                pc.push_match_position();
                return true;
            }
        };

        class add_match_parse_node : public parse_node {
        public:
            add_match_parse_node(MatchId id) : m_id(id) {
            }

            MatchId get_id() const noexcept {
                return m_id;
            }

            bool parse(parse_context& pc) const override {
                pc.add_match(m_id);
                return true;
            }

        private:
            MatchId m_id;
        };

        class end_match_parse_node : public parse_node {
        public:
            bool parse(parse_context& pc) const override {
                pc.pop_match_position();
                return true;
            }
        };

        class any_parse_node : public parse_node {
        public:
            bool parse(parse_context& pc) const override {
                if (pc.is_valid_iterator()) {
                    pc.increment_iterator();
                    return true;
                }
                return false;
            }
        };

        class end_parse_node : public parse_node {
        public:
            bool parse(parse_context& pc) const override {
                return pc.is_end_iterator();
            }
        };

        class newline_parse_node : public parse_node {
        public:
            newline_parse_node(const parse_node_ptr_type& child) : m_child(child) {
            }

            const parse_node_ptr_type& get_child() const noexcept {
                return m_child;
            }

            bool parse(parse_context& pc) const override {
                if (m_child->parse(pc)) {
                    pc.increment_line();
                    return true;
                }
                return false;
            }

        private:
            parse_node_ptr_type m_child;
        };

        template <class F>
        class function_parse_node : public parse_node {
        public:
            function_parse_node(const F& function) : m_function(function) {
            }

            const F& get_function() const noexcept {
                return m_function;
            }

            bool parse(parse_context& pc) const override {
                return m_function(pc);
            }

        private:
            F m_function;
        };

        class error_parse_node : public parse_node {
        public:
            error_parse_node(const parse_node_ptr_type& skip_node, ErrorId id) 
                : m_skip_node(skip_node), m_id(id)
            {
            }

            ErrorId get_id() const noexcept {
                return m_id;
            }

            const parse_node_ptr_type& get_skip_node() const noexcept {
                return m_skip_node;
            }

            bool parse(parse_context& pc) const override {
                const auto begin_iterator = pc.get_iterator();
                while (pc.is_valid_iterator()) {
                    const parse_context::state prev_state = pc.get_state();
                    if (m_skip_node->parse(pc)) {
                        break;
                    }
                    pc.set_state(prev_state);
                    pc.increment_iterator();
                }
                pc.add_error(begin_iterator, pc.get_iterator(), m_id);
                return true;
            }

        private:
            ErrorId m_id;
            parse_node_ptr_type m_skip_node;
        };

        class skip_before_parse_node : public parse_node {
        public:
            skip_before_parse_node(const parse_node_ptr_type& child) : m_child(child) {
            }

            const parse_node_ptr_type& get_child() const noexcept {
                return m_child;
            }

            bool parse(parse_context& pc) const override {
                while (pc.is_valid_iterator()) {
                    const auto before_parse_iterator = pc.get_iterator();
                    if (m_child->parse(pc)) {
                        pc.set_iterator(before_parse_iterator);
                        return true;
                    }
                    pc.increment_iterator();
                }
                return false;
            }

        private:
            parse_node_ptr_type m_child;
        };

        class skip_after_parse_node : public parse_node {
        public:
            skip_after_parse_node(const parse_node_ptr_type& child) : m_child(child) {
            }

            const parse_node_ptr_type& get_child() const noexcept {
                return m_child;
            }

            bool parse(parse_context& pc) const override {
                while (pc.is_valid_iterator()) {
                    if (m_child->parse(pc)) {
                        return true;
                    }
                    pc.increment_iterator();
                }
                return false;
            }

        private:
            parse_node_ptr_type m_child;
        };

        class debug_parse_node : public parse_node {
        public:
            debug_parse_node(const parse_node_ptr_type& child, const std::string& tag) : m_child(child), m_tag(tag) {
            }

            const parse_node_ptr_type& get_child() const noexcept {
                return m_child;
            }

            const std::string& get_tag() const noexcept {
                return m_tag;
            }

            bool parse(parse_context& pc) const override {
                return m_child->parse(pc);
            }

        private:
            parse_node_ptr_type m_child;
            std::string m_tag;
        };

        class rule;

        class rule_parse_node : public parse_node {
        public:
            rule_parse_node(rule& r) : m_rule(r) {
            }

            rule& get_rule() const {
                return m_rule;
            }

            bool parse(parse_context& pc) const override {
                return m_rule.parse(pc);
            }

        private:
            rule& m_rule;
        };

        class parse_node_ptr : public parse_node_ptr_type {
        public:
            parse_node_ptr() {
            }

            template <class T>
            parse_node_ptr(const std::shared_ptr<T>& node)
                : parse_node_ptr_type(node)
            {
            }

            template <class Symbol, std::enable_if_t<!std::is_invocable_v<std::decay_t<Symbol>, parse_context&>, bool> = true>
            parse_node_ptr(Symbol symbol) 
                : parse_node_ptr_type(std::make_shared<symbol_parse_node<Symbol>>(symbol))
            {
            }

            template <class F, std::enable_if_t<std::is_invocable_v<std::decay_t<F>, parse_context&>, bool> = true>
            parse_node_ptr(const F& function) 
                : parse_node_ptr_type(std::make_shared<function_parse_node<F>>(function))
            {
            }

            template <class Symbol>
            parse_node_ptr(const std::basic_string<Symbol>& string) 
                : parse_node_ptr_type(std::make_shared<string_parse_node<Symbol>>(string))
            {
            }

            template <class Symbol>
            parse_node_ptr(const Symbol* string)
                : parse_node_ptr(std::basic_string<Symbol>(string))
            {
            }

            parse_node_ptr(rule& r)
                : parse_node_ptr_type(std::make_shared<rule_parse_node>(r))
            {
            }

            bool parse(parse_context& pc) const {
                return (*this)->parse(pc);
            }
        };

        class rule {
        public:
            rule() {
            }

            template <class T>
            rule(T&& node) : m_node(parse_node_ptr(std::forward<T>(node))) {
            }

            bool parse(parse_context& pc) const {
                return m_node->parse(pc);
            }

        private:
            parse_node_ptr_type m_node;
        };

        template <class Symbol>
        static parse_node_ptr term(Symbol symbol) {
            return symbol;
        }

        template <class Symbol>
        static parse_node_ptr term(const Symbol* string) {
            return string;
        }

        template <class Symbol>
        static parse_node_ptr set(const Symbol* string) {
            std::basic_string<Symbol> str{ string };
            std::vector<Symbol> vec{ str.begin(), str.end() };
            return std::make_shared<set_parse_node<Symbol>>(vec);
        }

        template <class Symbol>
        static parse_node_ptr range(Symbol min, Symbol max) {
            return std::make_shared<range_parse_node<Symbol>>(min, max);
        }

        friend parse_node_ptr operator *(const parse_node_ptr& node) {
            return std::make_shared<loop_0_parse_node>(node);
        }

        friend parse_node_ptr operator +(const parse_node_ptr& node) {
            return std::make_shared<loop_1_parse_node>(node);
        }

        friend parse_node_ptr operator *(const parse_node_ptr& node, size_t count) {
            return std::make_shared<loop_n_parse_node>(node, count);
        }

        friend parse_node_ptr operator -(const parse_node_ptr& node) {
            return std::make_shared<optional_parse_node>(node);
        }

        friend parse_node_ptr operator &(const parse_node_ptr& node) {
            return std::make_shared<logical_and_parse_node>(node);
        }

        friend parse_node_ptr operator !(const parse_node_ptr& node) {
            return std::make_shared<logical_not_parse_node>(node);
        }

        template <class T>
        static void add_nodes(std::vector<std::shared_ptr<parse_node>>& nodes, const std::shared_ptr<parse_node>& child) {
            const T* parent = dynamic_cast<const T*>(child.get());
            if (parent) {
                nodes.insert(nodes.end(), parent->get_children().begin(), parent->get_children().end());
            }
            else {
                nodes.push_back(child);
            }
        }

        friend parse_node_ptr operator >>(const parse_node_ptr& left, const parse_node_ptr& right) {
            std::vector<std::shared_ptr<parse_node>> children;
            add_nodes<sequence_parse_node>(children, left);
            add_nodes<sequence_parse_node>(children, right);
            return std::make_shared<sequence_parse_node>(std::move(children));
        }

        friend parse_node_ptr operator -(const parse_node_ptr& left, const parse_node_ptr& right) {
            return !right >> left;
        }

        friend parse_node_ptr operator |(const parse_node_ptr& left, const parse_node_ptr& right) {
            std::vector<std::shared_ptr<parse_node>> children;
            add_nodes<choice_parse_node>(children, left);
            add_nodes<choice_parse_node>(children, right);
            return std::make_shared<choice_parse_node>(std::move(children));
        }

        friend parse_node_ptr operator ->*(const parse_node_ptr& node, MatchId id) {
            return std::make_shared<match_parse_node>(node, id);
        }

        static parse_node_ptr begin_match() {
            return std::make_shared<begin_match_parse_node>();
        }

        static parse_node_ptr add_match(MatchId id) {
            return std::make_shared<add_match_parse_node>(id);
        }

        static parse_node_ptr match(MatchId id) {
            return add_match(id);
        }

        static parse_node_ptr end_match() {
            return std::make_shared<end_match_parse_node>();
        }

        static parse_node_ptr any() {
            return std::make_shared<any_parse_node>();
        }

        static parse_node_ptr end() {
            return std::make_shared<end_parse_node>();
        }

        static parse_node_ptr newline(const parse_node_ptr& node) {
            return std::make_shared<newline_parse_node>(node);
        }

        template <class F>
        static parse_node_ptr function(const F& function) {
            return std::make_shared<function_parse_node<F>>(function);
        }

        static parse_node_ptr error(const parse_node_ptr& skip_node, ErrorId id) {
            return std::make_shared<error_parse_node>(skip_node, id);
        }

        static parse_node_ptr skip_before(const parse_node_ptr& node) {
            return std::make_shared<skip_before_parse_node>(node);
        }

        static parse_node_ptr skip_after(const parse_node_ptr& node) {
            return std::make_shared<skip_after_parse_node>(node);
        }

        static parse_node_ptr debug(const parse_node_ptr& node, const std::string& tag = {}) {
            return std::make_shared<debug_parse_node>(node, tag);
        }

        template <class... T>
        static parse_node_ptr left_associative(const parse_node_ptr& base, const T&... branches) {
            return begin_match() >> base >> *(... | parse_node_ptr(branches)) >> end_match();
        }

    }; //class parser


} //namespace parserlib


#endif //PARSERLIB_HPP
