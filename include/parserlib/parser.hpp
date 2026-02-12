#ifndef PARSERLIB_PARSER_HPP
#define PARSERLIB_PARSER_HPP


#include <cassert>
#include <memory>
#include <string>
#include <algorithm>
#include <map>
#include <set>
#include <type_traits>
#include "parse_context.hpp"
#include "vector.hpp"


namespace parserlib {


    template <class ParseContext = parse_context<>> class parser {
    public:
        class false_parse_node;
        class true_parse_node;
        class rule;

        class parse_node {
        public:
            virtual ~parse_node() {
            }

            virtual bool parse(ParseContext& pc) const = 0;
        };

        class parse_node_ptr : public std::shared_ptr<parse_node> {
        public:
            using std::shared_ptr<parse_node>::shared_ptr;

            template <class T> parse_node_ptr(const T& symbol) 
                : std::shared_ptr<parse_node>(std::make_shared<symbol_parse_node<T>>(symbol))
            {
            }

            template <class T> parse_node_ptr(const T* string) 
                : std::shared_ptr<parse_node>(std::make_shared<string_parse_node<T>>(string))
            {
            }

            parse_node_ptr(const rule& r) 
                : std::shared_ptr<parse_node>(r.get_ref_node())
            {
            }

            parse_node_ptr(bool b)
                : std::shared_ptr<parse_node>(create_bool_parse_node(b))
            {
            }

            parse_node_ptr operator *() const {
                return std::make_shared<loop_0_parse_node>(*this);
            }

            parse_node_ptr operator +() const {
                return std::make_shared<loop_1_parse_node>(*this);
            }

            parse_node_ptr operator -() const {
                return std::make_shared<optional_parse_node>(*this);
            }

            parse_node_ptr operator &() const {
                return std::make_shared<logical_and_parse_node>(*this);
            }

            parse_node_ptr operator !() const {
                return std::make_shared<logical_not_parse_node>(*this);
            }

            bool parse(ParseContext& pc) const {
                return (*this)->parse(pc);
            }

        private:
            static std::shared_ptr<parse_node> create_bool_parse_node(bool b) {
                return b ? 
                    std::static_pointer_cast<parse_node>(std::make_shared<true_parse_node>()) : 
                    std::static_pointer_cast<parse_node>(std::make_shared<false_parse_node>());
            }
        };

        using parse_node_vector = std::vector<parse_node_ptr>;


        class parent_parse_node_single_child : public parse_node {
        public:
            parent_parse_node_single_child(const parse_node_ptr& child) : m_child(child) {
            }

            const parse_node_ptr& get_child() const {
                return m_child;
            }

        private:
            const parse_node_ptr m_child;
        };

        class parent_parse_node : public parse_node {
        public:
            parent_parse_node(const parse_node_vector& children) : m_children(children) {
            }

            const parse_node_vector& get_children() const {
                return m_children;
            }

        private:
            const parse_node_vector m_children;
        };

        template <class T> class symbol_parse_node : public parse_node {
        public:
            symbol_parse_node(const T& symbol) : m_symbol(symbol) {
            }

            bool parse(ParseContext& pc) const override {
                if (pc.is_valid_iterator()) {
                    const auto& token = *pc.get_iterator();
                    if (ParseContext::compare(token, m_symbol) == 0) {
                        pc.increment_iterator();
                        return true;
                    }
                }
                return false;
            }

        private:
            const T m_symbol;
        };

        template <class T> class string_parse_node : public parse_node {
        public:
            string_parse_node(const std::basic_string<T>& string) : m_string(string) {
            }

            bool parse(ParseContext& pc) const override {
                if (pc.is_valid_iterator()) {
                    auto itStr = m_string.begin();
                    auto itSrc = pc.get_iterator();
                    for (;;) {
                        if (itStr == m_string.end()) {
                            pc.increment_iterator(m_string.size());
                            return true;
                        }
                        if (itSrc == pc.get_end_iterator()) {
                            break;
                        }
                        const auto& strch = *itStr;
                        const auto& token = *itSrc;
                        if (ParseContext::compare(token, strch) != 0) {
                            break;
                        }
                        ++itStr;
                        ++itSrc;
                    }
                }
                return false;
            }

        private:
            const std::basic_string<T> m_string;
        };

        template <class T> class set_parse_node : public parse_node {
        public:
            set_parse_node(const std::basic_string<T>& set) : m_set(get_sorted_vector(set)) {
            }

            bool parse(ParseContext& pc) const override {
                if (pc.is_valid_iterator()) {
                    const auto& token = *pc.get_iterator();
                    auto it = std::upper_bound(m_set.begin(), m_set.end(), token, [](const auto& a, const auto& b) {
                        return ParseContext::compare(a, b) < 0;
                    });
                    if (it != m_set.begin()) {
                        --it;
                        if (ParseContext::compare(token, *it) == 0) {
                            pc.increment_iterator();
                            return true;
                        }
                    }
                }
                return false;
            }

        private:
            const std::vector<T> m_set;

            static std::vector<T> get_sorted_vector(const std::basic_string<T>& set) {
                std::vector<T> result(set.begin(), set.end());
                std::sort(result.begin(), result.end());
                return result;
            }
        };

        template <class T> class range_parse_node : public parse_node {
        public:
            range_parse_node(const T& min, const T& max) : m_min(min), m_max(max) {
                assert(min <= max);
            }

            bool parse(ParseContext& pc) const override {
                if (pc.is_valid_iterator()) {
                    const auto& token = *pc.get_iterator();
                    if (ParseContext::compare(token, m_min) >= 0 && ParseContext::compare(token, m_max) <= 0) {
                        pc.increment_iterator();
                        return true;
                    }
                }
                return false;
            }

        private:
            const T m_min;
            const T m_max;
        };

        class loop_0_parse_node : public parent_parse_node_single_child {
        public:
            using parent_parse_node_single_child::parent_parse_node_single_child;

            bool parse(ParseContext& pc) const override {
                for (;;) {
                    const auto state = pc.get_state();
                    try {
                        const parse_node_ptr& child = this->get_child();
                        if (!child->parse(pc) || pc.get_iterator() == state.get_iterator()) {
                            pc.set_state(state);
                            break;
                        }
                    }
                    catch (...) {
                        pc.set_state(state);
                        throw;
                    }
                }
                return true;
            }
        };

        class loop_1_parse_node : public parent_parse_node_single_child {
        public:
            using parent_parse_node_single_child::parent_parse_node_single_child;

            bool parse(ParseContext& pc) const override {
                const auto state = pc.get_state();
                try {
                    const parse_node_ptr& child = this->get_child();
                    if (!child->parse(pc)) {
                        pc.set_state(state);
                        return false;
                    }
                }
                catch (...) {
                    pc.set_state(state);
                    throw;
                }

                for (;;) {
                    const auto state = pc.get_state();
                    try {
                        const parse_node_ptr& child = this->get_child();
                        if (!child->parse(pc) || pc.get_iterator() == state.get_iterator()) {
                            pc.set_state(state);
                            break;
                        }
                    }
                    catch (...) {
                        pc.set_state(state);
                        throw;
                    }
                }

                return true;
            }
        };

        class loop_n_parse_node : public parent_parse_node_single_child {
        public:
            loop_n_parse_node(size_t count, const parse_node_ptr& child)
                : parent_parse_node_single_child(child)
                , m_count(count)
            {
            }

            bool parse(ParseContext& pc) const override {
                const auto state = pc.get_state();
                try {
                    for (size_t i = 0; i < m_count; ++i) {
                        const parse_node_ptr& child = this->get_child();
                        if (!child->parse(pc)) {
                            pc.set_state(state);
                            return false;
                        }
                    }
                }
                catch (...) {
                    pc.set_state(state);
                    throw;
                }
                return true;
            }

        private:
            const size_t m_count;
        };

        class optional_parse_node : public parent_parse_node_single_child {
        public:
            using parent_parse_node_single_child::parent_parse_node_single_child;

            bool parse(ParseContext& pc) const override {
                const auto state = pc.get_state();
                try {
                    const parse_node_ptr& child = this->get_child();
                    if (!child->parse(pc)) {
                        pc.set_state(state);
                    }
                }
                catch (...) {
                    pc.set_state(state);
                    throw;
                }
                return true;
            }
        };

        class logical_and_parse_node : public parent_parse_node_single_child {
        public:
            using parent_parse_node_single_child::parent_parse_node_single_child;

            bool parse(ParseContext& pc) const override {
                const auto state = pc.get_state();
                try {
                    const parse_node_ptr& child = this->get_child();
                    const bool result = child->parse(pc);
                    pc.set_state(state);
                    return result;
                }
                catch (...) {
                    pc.set_state(state);
                    throw;
                }
            }
        };

        class logical_not_parse_node : public parent_parse_node_single_child {
        public:
            using parent_parse_node_single_child::parent_parse_node_single_child;

            bool parse(ParseContext& pc) const override {
                const auto state = pc.get_state();
                try {
                    const parse_node_ptr& child = this->get_child();
                    const bool result = !child->parse(pc);
                    pc.set_state(state);
                    return result;
                }
                catch (...) {
                    pc.set_state(state);
                    throw;
                }
            }
        };

        class sequence_parse_node : public parent_parse_node {
        public:
            using parent_parse_node::parent_parse_node;

            bool parse(ParseContext& pc) const override {
                const auto state = pc.get_state();
                try {
                    for (const parse_node_ptr& child : this->get_children()) {
                        if (!child->parse(pc)) {
                            pc.set_state(state);
                            return false;
                        }
                    }
                    return true;
                }
                catch (...) {
                    pc.set_state(state);
                    throw;
                }
            }
        };

        class choice_parse_node : public parent_parse_node {
        public:
            using parent_parse_node::parent_parse_node;

            bool parse(ParseContext& pc) const override {
                for (const parse_node_ptr& child : this->get_children()) {
                    const auto state = pc.get_state();
                    try {
                        if (child->parse(pc)) {
                            return true;
                        }
                    }
                    catch (...) {
                        pc.set_state(state);
                        throw;
                    }
                    pc.set_state(state);
                }
                return false;
            }
        };

        class match_parse_node : public parent_parse_node_single_child {
        public:
            match_parse_node(typename ParseContext::match_id id, const parse_node_ptr& child) 
                : parent_parse_node_single_child(child), m_id(id)
            {
            }

            bool parse(ParseContext& pc) const override {
                const auto match_start_state = pc.get_match_start_state();
                const parse_node_ptr& child = this->get_child();
                if (child->parse(pc)) {
                    pc.add_match(m_id, match_start_state);
                    return true;
                }
                return false;
            }

        private:
            typename ParseContext::match_id m_id;
        };

        class any_parse_node : public parse_node {
        public:
            bool parse(ParseContext& pc) const override {
                if (pc.is_valid_iterator()) {
                    pc.increment_iterator();
                    return true;
                }
                return false;
            }
        };

        class end_parse_node : public parse_node {
        public:
            bool parse(ParseContext& pc) const override {
                return pc.is_end_iterator();
            }
        };

        class false_parse_node : public parse_node {
        public:
            bool parse(ParseContext& pc) const override {
                return false;
            }
        };

        class true_parse_node : public parse_node {
        public:
            bool parse(ParseContext& pc) const override {
                return true;
            }
        };

        class newline_parse_node : public parent_parse_node_single_child {
        public:
            using parent_parse_node_single_child::parent_parse_node_single_child;

            bool parse(ParseContext& pc) const override {
                const parse_node_ptr& child = this->get_child();
                if (child->parse(pc)) {
                    pc.increment_line();
                    return true;
                }
                return false;
            }
        };

        template <class T> class function_parse_node : public parse_node {
        public:
            function_parse_node(const T& function) : m_function(function) {
            }

            bool parse(ParseContext& pc) const override {
                return m_function(pc);
            }

        private:
            const T m_function;
        };

        class ref_parse_node : public parse_node {
        public:
            ref_parse_node(const parse_node_ptr& child) : m_child(child) {
            }

            void set_child(const parse_node_ptr& child) {
                m_child = child;
            }

            bool parse(ParseContext& pc) const override {
                const std::shared_ptr<parse_node> child = m_child.lock();
                return child->parse(pc);
            }

        private:
            std::weak_ptr<parse_node> m_child;
        };

        class rule {
        public:
            rule() {
                set_constructed();
            }

            rule(const rule& r) : m_node(r.m_node) {
                set_constructed();
                update_refs();
            }

            rule(rule&&) = delete;

            rule(const parse_node_ptr& node) : m_node(node) {
                set_constructed();
                update_refs();
            }

            ~rule() {
                reset_constructed();
                remove_refs();
            }

            rule& operator = (const rule& r) {
                m_node = r.m_node;
                update_refs();
                return *this;
            }

            rule& operator = (const parse_node_ptr& node) {
                m_node = node;
                update_refs();
                return *this;
            }

            parse_node_ptr get_ref_node() const {
                std::shared_ptr<ref_parse_node> ref_node = std::make_shared<ref_parse_node>(is_constructed() ? m_node : nullptr);
                get_rule_ref_map()[this].push_back(ref_node);
                return ref_node;
            }

            parse_node_ptr operator *() const {
                return std::make_shared<loop_0_parse_node>(get_ref_node());
            }

            parse_node_ptr operator +() const {
                return std::make_shared<loop_1_parse_node>(get_ref_node());
            }

            parse_node_ptr operator -() const {
                return std::make_shared<optional_parse_node>(get_ref_node());
            }

            parse_node_ptr operator &() const {
                return std::make_shared<logical_and_parse_node>(get_ref_node());
            }

            parse_node_ptr operator !() const {
                return std::make_shared<logical_not_parse_node>(get_ref_node());
            }

            bool parse(ParseContext& pc) const {
                return m_node->parse(pc);
            }

        private:
            using rule_set = std::set<const rule*>;
            using rule_ref_map = std::map<const rule*, std::vector<std::shared_ptr<ref_parse_node>>>;

            parse_node_ptr m_node;

            static rule_set& get_rule_set() {
                static rule_set set;
                return set;
            }

            static rule_ref_map& get_rule_ref_map() {
                static rule_ref_map map;
                return map;
            }

            bool is_constructed() const {
                return get_rule_set().find(this) != get_rule_set().end();
            }

            void set_constructed() const {
                get_rule_set().insert(this);
            }

            void reset_constructed() const {
                get_rule_set().erase(this);
            }

            void update_refs() const {
                const auto it = get_rule_ref_map().find(this);
                if (it != get_rule_ref_map().end()) {
                    for (const auto& ref : it->second) {
                        ref->set_child(m_node);
                    }
                }
            }

            void remove_refs() const {
                get_rule_ref_map().erase(this);
            }
        };

        template <class T> static parse_node_ptr terminal(const T& symbol) {
            return std::make_shared<symbol_parse_node<T>>(symbol);
        }

        template <class T> static parse_node_ptr terminal(const T* symbol) {
            return std::make_shared<string_parse_node<T>>(symbol);
        }

        template <class T> static parse_node_ptr set(const T* set) {
            return std::make_shared<set_parse_node<T>>(set);
        }

        template <class T> static parse_node_ptr range(const T& min, const T& max) {
            return std::make_shared<range_parse_node<T>>(min, max);
        }

        static parse_node_ptr any() {
            return std::make_shared<any_parse_node>();
        }

        static parse_node_ptr end() {
            return std::make_shared<end_parse_node>();
        }

        static parse_node_ptr false_() {
            return std::make_shared<false_parse_node>();
        }

        static parse_node_ptr true_() {
            return std::make_shared<true_parse_node>();
        }

        static parse_node_ptr newline(const parse_node_ptr& child) {
            return std::make_shared<newline_parse_node>(child);
        }

        template <class T> static parse_node_ptr function(const T& function) {
            return std::make_shared<function_parse_node<T>>(function);
        }

        template <class T> static parse_node_ptr make_parse_node(const T& val) {
            return val;
        }

        friend parse_node_ptr operator >> (const parse_node_ptr& left, const parse_node_ptr& right) {
            const sequence_parse_node* left_sequence = dynamic_cast<const sequence_parse_node*>(left.get());
            const sequence_parse_node* right_sequence = dynamic_cast<const sequence_parse_node*>(right.get());
            if (left_sequence && right_sequence) {
                return std::make_shared<sequence_parse_node>(vector_cat(left_sequence->get_children(), right_sequence->get_children()));
            }
            else if (left_sequence) {
                return std::make_shared<sequence_parse_node>(vector_cat(left_sequence->get_children(), right));
            }
            else if (right_sequence) {
                return std::make_shared<sequence_parse_node>(vector_cat(left, right_sequence->get_children()));
            }
            else {
                return std::make_shared<sequence_parse_node>(vector_cat(left, right));
            }
        }

        friend parse_node_ptr operator | (const parse_node_ptr& left, const parse_node_ptr& right) {
            const choice_parse_node* left_choice = dynamic_cast<const choice_parse_node*>(left.get());
            const choice_parse_node* right_choice = dynamic_cast<const choice_parse_node*>(right.get());
            if (left_choice && right_choice) {
                return std::make_shared<choice_parse_node>(vector_cat(left_choice->get_children(), right_choice->get_children()));
            }
            else if (left_choice) {
                return std::make_shared<choice_parse_node>(vector_cat(left_choice->get_children(), right));
            }
            else if (right_choice) {
                return std::make_shared<choice_parse_node>(vector_cat(left, right_choice->get_children()));
            }
            else {
                return std::make_shared<choice_parse_node>(vector_cat(left, right));
            }
        }

        friend parse_node_ptr operator - (const parse_node_ptr& left, const parse_node_ptr& right) {
            return !right >> left;
        }

        friend parse_node_ptr operator * (size_t count, const parse_node_ptr& node) {
            return std::make_shared<loop_n_parse_node>(count, node);
        }

        friend parse_node_ptr operator ->* (const parse_node_ptr& node, typename ParseContext::match_id id) {
            return std::make_shared<match_parse_node>(id, node);
        }

    }; //class parser


} //namespace parserlib


#endif //PARSERLIB_PARSER_HPP
