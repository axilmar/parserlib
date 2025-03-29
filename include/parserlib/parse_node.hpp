#ifndef PARSERLIB_PARSE_NODE_HPP
#define PARSERLIB_PARSE_NODE_HPP


#include <type_traits>
#include <string>
#include <string_view>
#include <vector>
#include <set>
#include <unordered_set>
#include <variant>
#include <cassert>


namespace parserlib {


    class parse_node_base {
    };


    template <class Derived> class parse_node;
    template <class ParseContext> class rule;
    template <class ParseContext> class rule_ref_parse_node;
    template <class Terminal> class terminal_parse_node;
    template <class Terminal> class terminal_sequence_parse_node;
    template <class Terminal> class terminal_choice_parse_node;


    template <class Parser> class zero_or_more_parse_node;
    template <class Parser> class one_or_more_parse_node;
    template <class Parser> class optional_parse_node;
    template <class Parser> class logical_and_parse_node;
    template <class Parser> class logical_not_parse_node;


    template <class Derived>
    const Derived& get_parse_node_wrapper(const parse_node<Derived>& n) noexcept {
        return static_cast<const Derived&>(n);
    }


    template <class ParseContext>
    rule_ref_parse_node<ParseContext> get_parse_node_wrapper(rule<ParseContext>& r) noexcept {
        return r;
    }


    template <class Terminal, std::enable_if_t<!std::is_base_of_v<parse_node_base, Terminal>, bool> = true> 
    terminal_parse_node<Terminal> get_parse_node_wrapper(const Terminal& t) noexcept {
        return t;
    }


    template <class Char>
    terminal_sequence_parse_node<Char> get_parse_node_wrapper(const Char* str) noexcept {
        const Char* str_end = str;
        for(; *str_end; ++str_end) {}
        return { str, str_end };
    }


    template <class Char, class Traits, class Alloc>
    terminal_sequence_parse_node<Char> get_parse_node_wrapper(const std::basic_string<Char, Traits, Alloc>& str) noexcept {
        return { str.begin(), str.end() };
    }


    template <class K, class Pr, class Alloc>
    terminal_choice_parse_node<K> get_parse_node_wrapper(const std::set<K, Pr, Alloc>& set) noexcept {
        return { set.begin(), set.end() };
    }


    template <class K, class Hasher, class KeyEq, class Alloc>
    terminal_choice_parse_node<K> get_parse_node_wrapper(const std::unordered_set<K, Hasher, KeyEq, Alloc>& set) noexcept {
        return { set.begin(), set.end() };
    }


    template <class Derived>
    class parse_node : public parse_node_base {
    public:
        auto operator *() const noexcept;
        auto operator +() const noexcept;
        auto operator -() const noexcept;
        auto operator &() const noexcept;
        auto operator !() const noexcept;
    };


    template <class Derived> 
    auto parse_node<Derived>::operator *() const noexcept {
        return zero_or_more_parse_node(get_parse_node_wrapper(*this));
    }


    template <class Derived>
    auto parse_node<Derived>::operator +() const noexcept {
        return one_or_more_parse_node(get_parse_node_wrapper(*this));
    }


    template <class Derived>
    auto parse_node<Derived>::operator -() const noexcept {
        return optional_parse_node(get_parse_node_wrapper(*this));
    }


    template <class Derived>
    auto parse_node<Derived>::operator &() const noexcept {
        return logical_and_parse_node(get_parse_node_wrapper(*this));
    }


    template <class Derived>
    auto parse_node<Derived>::operator !() const noexcept {
        return logical_not_parse_node(get_parse_node_wrapper(*this));
    }


    struct left_recursion {
        void* rule;
    };


    class parse_result {
        using variant_type = std::variant<std::false_type, std::true_type, struct left_recursion>;

    public:
        enum value_type {
            FALSE,
            TRUE,
            LEFT_RECURSION
        };

        parse_result(bool value) noexcept 
            : m_value(value ? variant_type(std::true_type()) : variant_type(std::false_type()))
        {
        }

        parse_result(const left_recursion& lr) noexcept 
            : m_value(lr)
        {
        }

        bool is_false() const noexcept {
            return m_value.index() == 0;
        }

        bool is_true() const noexcept {
            return m_value.index() == 1;
        }

        bool is_left_recursion() const noexcept {
            return m_value.index() == 2;
        }

        void* left_recursion_rule() const noexcept {
            assert(is_left_recursion());
            return std::get<2>(m_value).rule;
        }

        operator bool() const noexcept {
            return is_true();
        }

        bool operator == (bool value) const noexcept {
            return is_true() == value;
        }

        bool operator != (bool value) const noexcept {
            return is_true() != value;
        }

        value_type value() const noexcept {
            return static_cast<value_type>(m_value.index());
        }

        operator value_type() const noexcept {
            return value();
        }

    private:
        const variant_type m_value;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_NODE_HPP
