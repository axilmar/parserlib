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
#include <stdexcept>


namespace parserlib {


    /**
     * Base class for parse nodes.
     * Used for identifying when a class is a parse node.
     */
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
    template <class Parser, class Callback> class callback_parse_node;
    class bool_parse_node;


    /**
     * Unwraps a parse node to its derived type.
     * @param n the parse node to unwrap.
     * @return reference to the derived type.
     */
    template <class Derived>
    const Derived& get_parse_node_wrapper(const parse_node<Derived>& n) noexcept {
        return static_cast<const Derived&>(n);
    }


    /**
     * Since rules are managed via non-const references,
     * Wrapping a rule means to create a rule reference parse node with a reference to the rule.
     * This allows recursion.
     * @param r rule to create a rule reference for.
     * @return a rule reference parse node.
     */
    template <class ParseContext>
    rule_ref_parse_node<ParseContext> get_parse_node_wrapper(rule<ParseContext>& r) noexcept {
        return r;
    }


    /**
     * Creates a terminal for a value that is not a parse node.
     * @param t value to create a terminal parse node for.
     * @return a terminal parse node.
     */
    template <class Terminal, std::enable_if_t<!std::is_base_of_v<parse_node_base, Terminal> && !std::is_pointer_v<Terminal>, bool> = true>
    terminal_parse_node<Terminal> get_parse_node_wrapper(const Terminal& t) noexcept {
        return t;
    }


    /**
     * Creates a terminal for a null-terminated string.
     * @param str null-terminated string to create a terminal parse node for.
     * @return a terminal parse node for a string.
     */
    template <class Char, std::enable_if_t<!std::is_function_v<Char>, bool> = true>
    terminal_sequence_parse_node<Char> get_parse_node_wrapper(const Char* str) noexcept {
        const Char* str_end = str;
        for(; *str_end; ++str_end) {}
        return { str, str_end };
    }


    /**
     * Creates a terminal for an std::basic_string.
     * @param str string to create a terminal parse node for.
     * @return a terminal parse node for a string.
     */
    template <class Char, class Traits, class Alloc>
    terminal_sequence_parse_node<Char> get_parse_node_wrapper(const std::basic_string<Char, Traits, Alloc>& str) noexcept {
        return { str.begin(), str.end() };
    }


    /**
     * Creates a terminal for an std::set.
     * @param set set to create a terminal parse node for.
     * @return a terminal parse node for a set.
     */
    template <class K, class Pr, class Alloc>
    terminal_choice_parse_node<K> get_parse_node_wrapper(const std::set<K, Pr, Alloc>& set) noexcept {
        return { set.begin(), set.end() };
    }


    /**
     * Creates a terminal for an std::unordered_set.
     * @param set set to create a terminal parse node for.
     * @return a terminal parse node for an unordered set.
     */
    template <class K, class Hasher, class KeyEq, class Alloc>
    terminal_choice_parse_node<K> get_parse_node_wrapper(const std::unordered_set<K, Hasher, KeyEq, Alloc>& set) noexcept {
        return { set.begin(), set.end() };
    }


    //forward declarations
    bool_parse_node get_parse_node_wrapper(bool value) noexcept;
    template <class R, class T> auto get_parse_node_wrapper(R(*f)(T) noexcept) noexcept;
    template <class R, class T> auto get_parse_node_wrapper(R(*f)(T)) noexcept;


    /**
     * Parse node wrapper type.
     * @param T type of value to get the parse node wrapper type of.
     */
    template <class T>
    using parse_node_wrapper_type = std::decay_t<decltype(get_parse_node_wrapper(*(T*)nullptr))>;


    /**
     * Value type for parse result that is left recursion.
     */
    struct left_recursion {
        void* rule;
    };


    /**
     * Parse result.
     */
    class parse_result {
        using variant_type = std::variant<std::false_type, std::true_type, struct left_recursion>;
    public:
        /**
         * Value type.
         */
        enum value_type {
            /** FALSE value type. */
            FALSE = 0,

            /** TRUE value type. */
            TRUE = 1,

            /** LEFT RECURSION value type. */
            LEFT_RECURSION = 2
        };

        /**
         * Creates a parse result for a boolean value.
         * @param value boolean value.
         */
        parse_result(bool value) noexcept
            : m_value(value ? variant_type(std::true_type()) : variant_type(std::false_type()))
        {
        }

        /**
         * Creates a parse result for left recursion.
         * @param lr left recursion result.
         */
        parse_result(const left_recursion& lr) noexcept
            : m_value(lr)
        {
        }

        /**
         * Checks if the result is false.
         * @return true if the result is false, false otherwise.
         */
        bool is_false() const noexcept {
            return m_value.index() == FALSE;
        }

        /**
         * Checks if the result is true.
         * It is true only if instantiated with a true boolean value.
         * Left recursion is considered false.
         * @return true if the result is true, false otherwise.
         */
        bool is_true() const noexcept {
            return m_value.index() == TRUE;
        }

        /**
         * Checks if the result is left recursion.
         * @return true if the result is left recursion, false otherwise.
         */
        bool is_left_recursion() const noexcept {
            return m_value.index() == LEFT_RECURSION;
        }

        void* left_recursion_rule() const noexcept {
            assert(is_left_recursion());
            return std::get<2>(m_value).rule;
        }

        /**
         * Checks if the result is true.
         * It is true only if instantiated with a true boolean value.
         * Left recursion is considered false.
         * @return true if the result is true, false otherwise.
         */
        operator bool() const noexcept {
            return is_true();
        }

        /**
         * Returns the value of the result.
         * @return the value of the result.
         */
        value_type value() const noexcept {
            return static_cast<value_type>(m_value.index());
        }

    private:
        const variant_type m_value;
    };


    /**
     * Typed base class for all parse nodes.
     * @param Derived type of the derived class.
     */
    template <class Derived>
    class parse_node : public parse_node_base {
    public:
        /**
         * Creates a zero or more parse node out of this node.
         * @return a zero or more parse node out of this node.
         */
        auto operator *() const noexcept;

        /**
         * Creates an one or more parse node out of this node.
         * @return an one or more parse node out of this node.
         */
        auto operator +() const noexcept;

        /**
         * Creates an optional parse node out of this node.
         * @return an optional parse node out of this node.
         */
        auto operator -() const noexcept;

        /**
         * Creates a logical AND parse node out of this node.
         * @return a logical AND parse node out of this node.
         */
        auto operator &() const noexcept;

        /**
         * Creates a logical OR parse node out of this node.
         * @return a logical OR parse node out of this node.
         */
        auto operator !() const noexcept;

        /**
         * Creates a callback parse node out of this node.
         * @return a callback parse node out of this node.
         */
        template <class Callback> auto operator [](const Callback& callback) const noexcept {
            return callback_parse_node(get_parse_node_wrapper(*this), callback);
        }

        /**
         * The default implementation of the function 'parse'.
         * @param pc the parse context.
         * @exception std::logic_error thrown because this must be respecified in subclasses.
         */
        template <class ParseContext>
        parse_result parse(ParseContext& pc) const noexcept {
            throw std::logic_error("Parse method not specified.");
        }

        /**
         * The default implementation of the function 'parse_left_recursion_start'.
         * An empty implementation.
         * @param pc the parse context.
         * @return false.
         */
        template <class ParseContext>
        parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
            return false;
        }

        /**
         * The default implementation of the function 'parse_left_recursion_continuation'.
         * An empty implementation.
         * @param pc the parse context.
         * @param match_start match start state.
         * @return false.
         */
        template <class ParseContext, class State>
        parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
            return false;
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_NODE_HPP
