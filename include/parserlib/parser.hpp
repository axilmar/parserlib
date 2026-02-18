#ifndef PARSERLIB_PARSER_HPP
#define PARSERLIB_PARSER_HPP


#include <string>
#include <cctype>
#include <vector>
#include <utility>
#include <memory>
#include <string_view>
#include <initializer_list>
#include <algorithm>
#include <cassert>
#include <map>
#include <set>
#include <stdexcept>


namespace parserlib {


class default_symbol_comparator {
public:
    template <class L, class R>
    static int compare(const L& left, const R& right) {
        return static_cast<int>(left) - static_cast<int>(right);
    }
};


class case_insensitive_symbol_comparator {
public:
    template <class L, class R>
    static int compare(const L& left, const R& right) {
        return std::tolower(static_cast<int>(left)) - std::tolower(static_cast<int>(right));
    }
};


enum class parse_result_type {
    left_recursion = -1,
    failure,
    success
};


class invalid_parse_result_type_error : public std::runtime_error {
public:
    invalid_parse_result_type_error(const char* what)
        : std::runtime_error(what)
    {
    }
};


template <class T> struct get_source_by_type;


template <> struct get_source_by_type<signed char> {
    template <class Iterator>
    static std::basic_string<signed char> get_source(const Iterator& begin, const Iterator& end) {
        return { begin, end };
    }
};


template <> struct get_source_by_type<unsigned char> {
    template <class Iterator>
    static std::basic_string<unsigned char> get_source(const Iterator& begin, const Iterator& end) {
        return { begin, end };
    }
};


template <> struct get_source_by_type<char> {
    template <class Iterator>
    static std::basic_string<char> get_source(const Iterator& begin, const Iterator& end) {
        return { begin, end };
    }
};


#ifdef char8_t
template <> struct get_source_by_type<char8_t> {
    template <class Iterator>
    static std::basic_string<char8_t> get_source(const Iterator& begin, const Iterator& end) {
        return { begin, end };
    }
};
#endif


template <> struct get_source_by_type<char16_t> {
    template <class Iterator>
    static std::basic_string<char16_t> get_source(const Iterator& begin, const Iterator& end) {
        return { begin, end };
    }
};


template <> struct get_source_by_type<char32_t> {
    template <class Iterator>
    static std::basic_string<char32_t> get_source(const Iterator& begin, const Iterator& end) {
        return { begin, end };
    }
};


template <
    class Iterator = std::string::const_iterator, 
    class MatchId = int, 
    class ErrorId = int, 
    class SymbolComparator = default_symbol_comparator>
class parser {
public:
    /**************************************************************************
        GET SOURCE
     **************************************************************************/

    class match;

    template <class T> struct get_source_by_type {
        template <class Iterator>
        static auto get_source(const Iterator& begin, const Iterator& end) {
            return parserlib::get_source_by_type<typename Iterator::value_type>::get_source(begin, end);
        }
    };
   
    template <> struct get_source_by_type<match> {
        template <class Iterator>
        static auto get_source(const Iterator& begin, const Iterator& end) {
            return parserlib::get_source_by_type<typename Iterator::value_type>::get_source(begin->begin(), std::prev(end)->end());
        }
    };   

    /**************************************************************************
        SOURCE PARTITION
     **************************************************************************/

    template <class Id>
    class source_partition {
    public:
        using id_type = Id;

        using iterator_type = Iterator;

        source_partition() {
        }

        source_partition(const Id& id, const Iterator& begin, const Iterator& end)
            : m_id(id)
            , m_begin(begin)
            , m_end(end)
        {
        }

        const Id& get_id() const {
            return m_id;
        }

        const Iterator& begin() const {
            return m_begin;
        }

        const Iterator& end() const {
            return m_end;
        }

        auto get_source() const {
            return get_source_by_type<typename Iterator::value_type>::get_source(m_begin, m_end);
        }

    private:
        Id m_id;
        Iterator m_begin;
        Iterator m_end;
    };

    /**************************************************************************
        MATCH
     **************************************************************************/

    class match : public source_partition<MatchId> {
    public:
        match() {
        }

        match(const MatchId& id, const Iterator& begin, const Iterator& end, std::vector<match>&& children)
            : source_partition<MatchId>(id, begin, end)
            , m_children(std::move(children))
        {
        }

        operator int() const {
            return static_cast<int>(this->get_id());
        }

        const std::vector<match>& get_children() const {
            return m_children;
        }

    private:
        std::vector<match> m_children;
    };

    using match_container = std::vector<match>;

    /**************************************************************************
        ERROR
     **************************************************************************/

    class error : public source_partition<ErrorId> {
    public:
        using source_partition<ErrorId>::source_partition;
    };

    using error_container = std::vector<error>;

    /**************************************************************************
        PARSE RESULT
     **************************************************************************/

    class rule_parse_node;

    class parse_result {
    public:
        parse_result(parse_result_type type)
            : m_type(type)
        {
        }

        parse_result(bool value)
            : m_type(value ? parse_result_type::success : parse_result_type::failure)
        {
        }

        parse_result_type get_type() const {
            return m_type;
        }

        operator bool() const {
            if (m_type == parse_result_type::success) {
                return true;
            }
            if (m_type == parse_result_type::failure) {
                return false;
            }
            throw invalid_parse_result_type_error("parse result cannot be converted to bool.");
        }

    private:
        parse_result_type m_type;
    };

    /**************************************************************************
        PARSE CONTEXT
     **************************************************************************/

    template <class Symbol>
    static int compare(const typename Iterator::value_type& token, const Symbol& symbol) {
        return SymbolComparator::compare(token, symbol);
    }

    class parse_context {
    public:
        class parse_state {
        public:
            parse_state(const Iterator& iterator)
                : m_iterator(iterator)
                , m_match_count(0)
            {
            }

        private:
            Iterator m_iterator;
            size_t m_match_count;
            friend class parse_context;
        };

        class state {
        public:
            state(const Iterator& begin, const Iterator& end)
                : m_parse_state(begin)
                , m_match_start_state(begin)
                , m_end(end)
            {
            }

        private:
            parse_state m_parse_state;
            parse_state m_match_start_state;
            Iterator m_end;
            friend class parse_context;
        };

        parse_context(const Iterator& begin, const Iterator& end) 
            : m_state(begin, end)
        {
        }

        template <class Container>
        parse_context(const Container& container)
            : parse_context(container.begin(), container.end())
        {
        }

        const state& get_state() const {
            return m_state;
        }

        void set_state(const state& st) {
            m_state = st;
            m_matches.resize(st.m_parse_state.m_match_count);
        }

        bool is_valid_iterator() const {
            return m_state.m_parse_state.m_iterator != m_state.m_end;
        }

        bool is_end_iterator() const {
            return m_state.m_parse_state.m_iterator == m_state.m_end;
        }

        const Iterator& get_iterator() const {
            return m_state.m_parse_state.m_iterator;
        }

        const Iterator& get_end_iterator() const {
            return m_state.m_end;
        }

        template <class Symbol>
        int compare(const typename Iterator::value_type& token, const Symbol& symbol) {
            return parser::compare(token, symbol);
        }

        void increment_iterator() {
            ++m_state.m_parse_state.m_iterator;
            m_state.m_match_start_state = m_state.m_parse_state;
        }

        void increment_iterator(size_t count) {
            m_state.m_parse_state.m_iterator += count;
            m_state.m_match_start_state = m_state.m_parse_state;
        }

        void increment_line() {
            m_state.m_parse_state.m_iterator.increment_line();
            m_state.m_match_start_state.m_iterator = m_state.m_parse_state.m_iterator;
        }

        const parse_state& get_match_start_state() const {
            return m_state.m_match_start_state;
        }

        const match_container& get_matches() const {
            return m_matches;
        }

        void add_match(const MatchId& id, const parse_state& from_state) {
            match_container children(m_matches.begin() + from_state.m_match_count, m_matches.end());
            m_matches.resize(from_state.m_match_count);
            m_matches.push_back(match(id, from_state.m_iterator, m_state.m_parse_state.m_iterator, std::move(children)));
            m_state.m_match_start_state.m_match_count = m_state.m_parse_state.m_match_count = m_matches.size();
        }

        template <class DerivedMatchId = int, class DerivedErrorId = int, class DerivedSymbolComparator = default_symbol_comparator>
        auto derive_parse_context() const {
            return parser<typename match_container::const_iterator, DerivedMatchId, DerivedErrorId, DerivedSymbolComparator>::parse_context(m_matches);
        }

        parse_result parse_left_recursion(const rule_parse_node& node) {
            //TODO
            return node.get_child().parse(*this);
        }

    private:
        state m_state;
        match_container m_matches;
    };

    /**************************************************************************
        PARSE NODE
     **************************************************************************/

    class parse_node {
    public:
        virtual ~parse_node() {
        }

        const std::string& get_name() const {
            return m_name;
        }

        void set_name(const std::string& name) {
            m_name = name;
        }

        virtual parse_result parse(parse_context& pc) const = 0;

    private:
        std::string m_name;
    };

    /**************************************************************************
        PARSE NODE PTR
     **************************************************************************/

    class rule;

    class parse_node_ptr {
    public:
        parse_node_ptr() {
        }

        template <class T>
        parse_node_ptr(const std::shared_ptr<T>& parse_node)
            : m_node(parse_node)
        {
        }

        template <class Symbol>
        parse_node_ptr(const Symbol& symbol)
            : parse_node_ptr(terminal(symbol))
        {
        }

        template <class Symbol>
        parse_node_ptr(const Symbol* string)
            : parse_node_ptr(terminal(string))
        {
        }

        parse_node_ptr(const rule& r)
            : m_node(r.get_ref_node())
        {
        }

        const parse_node* operator ->() const {
            return m_node.get();
        }

        const std::shared_ptr<parse_node>& get() const {
            return m_node;
        }

        const std::string& get_name() const {
            return m_node->get_name();
        }

        void set_name(const std::string& name) {
            m_node->set_name(name);
        }

        parse_result parse(parse_context& pc) const {
            return m_node->parse(pc);
        }

    private:
        std::shared_ptr<parse_node> m_node;
    };

    using parse_node_container = std::vector<parse_node_ptr>;

    /**************************************************************************
        PARENT PARSE NODES
     **************************************************************************/

    class parent_parse_node_1 : public parse_node {
    public:
        parent_parse_node_1(const parse_node_ptr& child = {})
            : m_child(child)
        {
        }

        const parse_node_ptr& get_child() const {
            return m_child;
        }

        void set_child(const parse_node_ptr& child) {
            m_child = child;
        }

    private:
        parse_node_ptr m_child;
    };

    class parent_parse_node : public parse_node {
    public:
        parent_parse_node(const parse_node_container& children = {})
            : m_children(std::move(children))
        {
        }

        parent_parse_node(parse_node_container&& children = {})
            : m_children(std::move(children))
        {
        }

        const parse_node_container& get_children() const {
            return m_children;
        }

        void set_children(const parse_node_container& children) {
            m_children = children;
        }

        void set_children(parse_node_container&& children) {
            m_children = std::move(children);
        }

    private:
        parse_node_container m_children;
    };

    /**************************************************************************
        SYMBOL PARSE NODE
     **************************************************************************/

    template <class Symbol>
    class symbol_parse_node : public parse_node {
    public:
        symbol_parse_node(const Symbol& symbol)
            : m_symbol(symbol)
        {
        }

        parse_result parse(parse_context& pc) const override {
            if (pc.is_valid_iterator()) {
                const auto& token = *pc.get_iterator();

                if (compare(token, m_symbol) == 0) {
                    pc.increment_iterator();
                    return parse_result_type::success;
                }
            }
            return parse_result_type::failure;
        }

    private:
        Symbol m_symbol;
    };

    /**************************************************************************
        STRING PARSE NODE
     **************************************************************************/

    template <class Symbol>
    class string_parse_node : public parse_node {
    public:
        string_parse_node(const Symbol* string)
            : m_string(string)
        {
        }

        parse_result parse(parse_context& pc) const override {
            if (pc.is_valid_iterator()) {
                auto strIt = m_string.begin();
                Iterator srcIt = pc.get_iterator();

                for (;;) {
                    if (strIt == m_string.end()) {
                        pc.increment_iterator(m_string.size());
                        return parse_result_type::success;
                    }
                    
                    if (srcIt == pc.get_end_iterator()) {
                        break;
                    }
                    
                    const auto token = *srcIt;
                    const auto char_ = *strIt;
                    if (compare(token, char_) != 0) {
                        break;
                    }

                    ++strIt;
                    ++srcIt;
                }
            }
            return parse_result_type::failure;
        }

    private:
        std::basic_string_view<Symbol> m_string;
    };

    /**************************************************************************
        SET PARSE NODE
     **************************************************************************/

    template <class Symbol>
    class set_parse_node : public parse_node {
    public:
        set_parse_node(const Symbol* set)
            : m_set(init_set_vector(set))
        {
        }

        set_parse_node(const std::initializer_list<Symbol>& set)
            : m_set(init_set_vector(set))
        {
        }

        parse_result parse(parse_context& pc) const override {
            if (pc.is_valid_iterator()) {
                const typename Iterator::value_type& token = *pc.get_iterator();

                auto it = std::upper_bound(m_set.begin(), m_set.end(), token, [](const typename Iterator::value_type& token, const Symbol& symbol) {
                    return compare(token, symbol) < 0;
                });

                if (it != m_set.begin()) {
                    --it;

                    if (compare(token, *it) == 0) {
                        pc.increment_iterator();
                        return parse_result_type::success;
                    }
                }
            }
            return parse_result_type::failure;
        }

    private:
        std::vector<Symbol> m_set;

        static std::vector<Symbol> init_set_vector(const Symbol* set) {
            std::basic_string_view<Symbol> view(set);
            std::vector<Symbol> result(view.begin(), view.end());
            std::sort(result.begin(), result.end());
            return result;
        }

        static std::vector<Symbol> init_set_vector(const std::initializer_list<Symbol>& set) {
            std::vector<Symbol> result(set.begin(), set.end());
            std::sort(result.begin(), result.end());
            return result;
        }
    };

    /**************************************************************************
        RANGE PARSE NODE
     **************************************************************************/

    template <class Symbol>
    class range_parse_node : public parse_node {
    public:
        range_parse_node(const Symbol& min, const Symbol& max)
            : m_min(min)
            , m_max(max)
        {
            assert(min <= max);
        }

        parse_result parse(parse_context& pc) const override {
            if (pc.is_valid_iterator()) {
                const typename Iterator::value_type& token = *pc.get_iterator();

                if (compare(token, m_min) >= 0 && compare(token, m_max) <= 0) {
                    pc.increment_iterator();
                    return parse_result_type::success;
                }
            }
            return parse_result_type::failure;
        }

    private:
        Symbol m_min;
        Symbol m_max;
    };

    /**************************************************************************
        LOOP 0 PARSE NODE
     **************************************************************************/

    class loop_0_parse_node : public parent_parse_node_1 {
    public:
        using parent_parse_node_1::parent_parse_node_1;

        parse_result parse(parse_context& pc) const override {
            const parse_node_ptr& child = this->get_child();

            for (;;) {
                const parse_result result = child.parse(pc);

                if (result.get_type() == parse_result_type::success) {
                    continue;
                }

                if (result.get_type() == parse_result_type::left_recursion) {
                    return result;
                }

                break;
            }

            return parse_result_type::success;
        }
    };

    /**************************************************************************
        LOOP 1 PARSE NODE
     **************************************************************************/

    class loop_1_parse_node : public loop_0_parse_node {
    public:
        using loop_1_parse_node::loop_1_parse_node;

        parse_result parse(parse_context& pc) const override {
            const parse_node_ptr& child = this->get_child();

            const parse_result result = child.parse(pc);

            if (result.get_type() == parse_result_type::success) {
                return loop_0_parse_node::parse(pc);
            }

            return result;
        }
    };

    /**************************************************************************
        OPTIONAL PARSE NODE
     **************************************************************************/

    class optional_parse_node : public parent_parse_node_1 {
    public:
        using parent_parse_node_1::parent_parse_node_1;

        parse_result parse(parse_context& pc) const override {
            const parse_node_ptr& child = this->get_child();
            
            const parse_result result = child.parse(pc);

            if (result.get_type() != parse_result_type::failure) {
                return result;
            }

            return parse_result_type::success;
        }
    };

    /**************************************************************************
        LOGICAL AND PARSE NODE
     **************************************************************************/

    class logical_and_parse_node : public parent_parse_node_1 {
    public:
        using parent_parse_node_1::parent_parse_node_1;

        parse_result parse(parse_context& pc) const override {
            const parse_node_ptr& child = this->get_child();

            const parse_context::state base_state = pc.get_state();
            const parse_result result = child.parse(pc);
            pc.set_state(base_state);

            return result;
        }
    };

    /**************************************************************************
        LOGICAL AND PARSE NODE
     **************************************************************************/

    class logical_not_parse_node : public parent_parse_node_1 {
    public:
        using parent_parse_node_1::parent_parse_node_1;

        parse_result parse(parse_context& pc) const override {
            const parse_node_ptr& child = this->get_child();

            const parse_context::state base_state = pc.get_state();
            const parse_result result = child.parse(pc);
            pc.set_state(base_state);
            
            if (result.get_type() == parse_result_type::success) {
                return parse_result_type::failure;
            }

            if (result.get_type() == parse_result_type::failure) {
                return parse_result_type::success;
            }

            return result;
        }
    };

    /**************************************************************************
        SEQUENCE PARSE NODE
     **************************************************************************/

    class sequence_parse_node : public parent_parse_node {
    public:
        using parent_parse_node::parent_parse_node;

        parse_result parse(parse_context& pc) const override {
            const parse_node_container& children = this->get_children();

            const parse_context::state base_state = pc.get_state();

            for (const parse_node_ptr& child : children) {
                const parse_result result = child.parse(pc);

                if (result.get_type() == parse_result_type::success) {
                    continue;
                }

                pc.set_state(base_state);
                return result;
            }

            return parse_result_type::success;
        }
    };

    /**************************************************************************
        CHOICE PARSE NODE
     **************************************************************************/

    class choice_parse_node : public parent_parse_node {
    public:
        using parent_parse_node::parent_parse_node;

        parse_result parse(parse_context& pc) const override {
            const parse_node_container& children = this->get_children();

            for (const parse_node_ptr& child : children) {
                const parse_result result = child.parse(pc);

                if (result.get_type() != parse_result_type::failure) {
                    return result;
                }
            }

            return parse_result_type::failure;
        }
    };

    /**************************************************************************
        MATCH PARSE NODE
     **************************************************************************/

    class match_parse_node : public parent_parse_node_1 {
    public:
        match_parse_node(const parse_node_ptr& child, const MatchId& id)
            : parent_parse_node_1(child)
            , m_id(id)
        {
        }

        parse_result parse(parse_context& pc) const override {
            const parse_node_ptr& child = this->get_child();

            const parse_context::parse_state from_state = pc.get_match_start_state();

            const parse_result result = child.parse(pc);
            
            if (result.get_type() == parse_result_type::success) {
                pc.add_match(m_id, from_state);
                return result;
            }

            return result;
        }

    private:
        MatchId m_id;
    };

    /**************************************************************************
        ANY PARSE NODE
     **************************************************************************/

    class any_parse_node : public parse_node {
    public:
        parse_result parse(parse_context& pc) const override {
            if (pc.is_valid_iterator()) {
                pc.increment_iterator();
                return parse_result_type::success;
            }
            return parse_result_type::failure;
        }
    };

    /**************************************************************************
        ANY PARSE NODE
     **************************************************************************/

    class end_parse_node : public parse_node {
    public:
        parse_result parse(parse_context& pc) const override {
            if (pc.is_end_iterator()) {
                return parse_result_type::success;
            }
            return parse_result_type::failure;
        }
    };

    /**************************************************************************
        FAILURE PARSE NODE
     **************************************************************************/

    class failure_parse_node : public parse_node {
    public:
        parse_result parse(parse_context& pc) const override {
            return parse_result_type::failure;
        }
    };

    /**************************************************************************
        SUCCESS PARSE NODE
     **************************************************************************/

    class success_parse_node : public parse_node {
    public:
        parse_result parse(parse_context& pc) const override {
            return parse_result_type::success;
        }
    };

    /**************************************************************************
        NEWLINE PARSE NODE
     **************************************************************************/

    class newline_parse_node : public parent_parse_node_1 {
    public:
        using parent_parse_node_1::parent_parse_node_1;

        parse_result parse(parse_context& pc) const override {
            const parse_node_ptr& child = this->get_child();

            const parse_result result = child->parse(pc);

            if (result.get_type() == parse_result_type::success) {
                pc.increment_line();
            }

            return result;
        }
    };

    /**************************************************************************
        FUNCTION PARSE NODE
     **************************************************************************/

    template <class F>
    class function_parse_node : public parse_node {
    public:
        function_parse_node(const F& function)
            : m_function(function)
        {
        }

        parse_result parse(parse_context& pc) const override {
            return m_function(pc);
        }

    private:
        F m_function;
    };

    /**************************************************************************
        RULE PARSE NODE
     **************************************************************************/

    class rule_parse_node : public parent_parse_node_1 {
    public:
        using parent_parse_node_1::parent_parse_node_1;

        parse_result parse(parse_context& pc) const override {
            return pc.parse_left_recursion(*this);
        }
    };

    /**************************************************************************
        RULE REF PARSE NODE
     **************************************************************************/

    class rule_ref_parse_node : public parse_node {
    public:
        rule_ref_parse_node(const std::shared_ptr<rule_parse_node>& pn)
            : m_parse_node(pn)
        {
        }

        parse_result parse(parse_context& pc) const override {
            std::shared_ptr<rule_parse_node> ptr = m_parse_node.lock();
            return ptr->parse(pc);
        }

    private:
        std::weak_ptr<rule_parse_node> m_parse_node;
    };

    /**************************************************************************
        RULE
     **************************************************************************/

    class rule {
    public:
        rule() 
            : m_node(init_node())
        {
        }

        rule(const rule& r)
            : m_node(init_node(r.m_node->get_child()))
        {
        }

        rule(rule&&) = delete;

        rule(const parse_node_ptr& ptr)
            : m_node(init_node(ptr))
        {
        }

        template <class Symbol>
        rule(const Symbol& symbol)
            : rule(parse_node_ptr(symbol))
        {
        }

        template <class Symbol>
        rule(const Symbol* string)
            : rule(parse_node_ptr(string))
        {
        }

        ~rule() {
            rule_map().erase(this);
        }

        rule& operator = (rule&&) = delete;

        rule& operator = (const parse_node_ptr& ptr) {
            m_node->set_child(ptr);
            return *this;
        }

        template <class Symbol>
        rule& operator = (const Symbol& symbol) {
            return operator = (parse_node_ptr(symbol));
        }

        template <class Symbol>
        rule& operator = (const Symbol* string) {
            return operator = (parse_node_ptr(string));
        }

        std::shared_ptr<rule_ref_parse_node> get_ref_node() const {
            return std::make_shared<rule_ref_parse_node>(get_or_create_node());
        }

        const std::string& get_name() const {
            return m_node->get_name();
        }

        void set_name(const std::string& name) {
            m_node->set_name(name);
        }

        parse_result parse(parse_context& pc) const {
            return m_node->parse(pc);
        }

    private:
        std::shared_ptr<rule_parse_node> m_node;

        static auto& rule_map() {
            static std::map<const rule*, std::shared_ptr<rule_parse_node>> map;
            return map;
        }

        std::shared_ptr<rule_parse_node> get_or_create_node() const {
            auto& map = rule_map();
            
            auto it = map.find(this);

            if (it != map.end()) {
                return it->second;
            }

            std::shared_ptr<rule_parse_node> node = std::make_shared<rule_parse_node>();
            return map[this] = node;

            return node;
        }

        std::shared_ptr<rule_parse_node> init_node(const parse_node_ptr& pn = {}) const {
            std::shared_ptr<rule_parse_node> node = get_or_create_node();
            node->set_child(pn);
            return node;
        }
    };

    /**************************************************************************
        OPERATORS 
     **************************************************************************/

    friend parse_node_ptr operator *(const parse_node_ptr& pn) {
        return std::make_shared<loop_0_parse_node>(pn);
    }

    friend parse_node_ptr operator +(const parse_node_ptr& pn) {
        return std::make_shared<loop_1_parse_node>(pn);
    }

    friend parse_node_ptr operator -(const parse_node_ptr& pn) {
        return std::make_shared<optional_parse_node>(pn);
    }

    friend parse_node_ptr operator &(const parse_node_ptr& pn) {
        return std::make_shared<logical_and_parse_node>(pn);
    }

    friend parse_node_ptr operator !(const parse_node_ptr& pn) {
        return std::make_shared<logical_not_parse_node>(pn);
    }

    friend parse_node_ptr operator >> (const parse_node_ptr& left, const parse_node_ptr& right) {
        const sequence_parse_node* left_sequence = dynamic_cast<const sequence_parse_node*>(left.get().get());
        const sequence_parse_node* right_sequence = dynamic_cast<const sequence_parse_node*>(right.get().get());
        parse_node_container children;
        if (left_sequence) {
            children.insert(children.end(), left_sequence->get_children().begin(), left_sequence->get_children().end());
        }
        else {
            children.push_back(left);
        }
        if (right_sequence) {
            children.insert(children.end(), right_sequence->get_children().begin(), right_sequence->get_children().end());
        }
        else {
            children.push_back(right);
        }
        return std::make_shared<sequence_parse_node>(std::move(children));
    }

    friend parse_node_ptr operator | (const parse_node_ptr& left, const parse_node_ptr& right) {
        const choice_parse_node* left_choice = dynamic_cast<const choice_parse_node*>(left.get().get());
        const choice_parse_node* right_choice = dynamic_cast<const choice_parse_node*>(right.get().get());
        parse_node_container children;
        if (left_choice) {
            children.insert(children.end(), left_choice->get_children().begin(), left_choice->get_children().end());
        }
        else {
            children.push_back(left);
        }
        if (right_choice) {
            children.insert(children.end(), right_choice->get_children().begin(), right_choice->get_children().end());
        }
        else {
            children.push_back(right);
        }
        return std::make_shared<choice_parse_node>(std::move(children));
    }

    friend parse_node_ptr operator - (const parse_node_ptr& left, const parse_node_ptr& right) {
        return !right >> left;
    }

    friend parse_node_ptr operator ->* (const parse_node_ptr& left, const MatchId& right) {
        return std::make_shared<match_parse_node>(left, right);
    }

    /**************************************************************************
        KEYWORDS
     **************************************************************************/

    template <class Symbol>
    static parse_node_ptr terminal(const Symbol& symbol) {
        return std::make_shared<symbol_parse_node<Symbol>>(symbol);
    }

    template <class Symbol>
    static parse_node_ptr terminal(const Symbol* string) {
        return std::make_shared<string_parse_node<Symbol>>(string);
    }

    template <class Symbol>
    static parse_node_ptr set(const Symbol* set) {
        return std::make_shared<set_parse_node<Symbol>>(set);
    }

    template <class Symbol>
    static parse_node_ptr set(const std::initializer_list<Symbol>& set) {
        return std::make_shared<set_parse_node<Symbol>>(set);
    }

    template <class Symbol>
    static parse_node_ptr range(const Symbol& min, const Symbol& max) {
        return std::make_shared<range_parse_node<Symbol>>(min, max);
    }

    static parse_node_ptr any() {
        return std::make_shared<any_parse_node>();
    }

    static parse_node_ptr end() {
        return std::make_shared<end_parse_node>();
    }

    static parse_node_ptr failure() {
        return std::make_shared<failure_parse_node>();
    }

    static parse_node_ptr success() {
        return std::make_shared<success_parse_node>();
    }

    static parse_node_ptr newline(const parse_node_ptr& pn) {
        return std::make_shared<newline_parse_node>(pn);
    }

    template <class F>
    static parse_node_ptr function(const F& function) {
        return std::make_shared<function_parse_node<F>>(function);
    }
};


} //namespace parserlib


#endif // PARSERLIB_PARSER_HPP
