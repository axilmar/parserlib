#ifndef PARSERLIB_TERMINAL_SET_PARSER_HPP
#define PARSERLIB_TERMINAL_SET_PARSER_HPP


#include <vector>
#include <set>
#include <initializer_list>
#include <algorithm>
#include <tuple>
#include <type_traits>
#include "parser.hpp"
#include "less.hpp"
#include "string_util.hpp"
#include "is_container.hpp"


namespace parserlib {


    /**
     * A parser that checks if a token exists in a set of terminal values.
     * @param Terminal terminal value.
     */
    template <class Terminal>
    class terminal_set_parser : public parser<terminal_set_parser<Terminal>> {
    public:
        /**
         * The constructor.
         * @param begin beginning of the container that represents the set of values.
         * @param end end of the container that represents the set of values.
         */
        template <class Iterator>
        terminal_set_parser(const Iterator& begin, const Iterator& end) noexcept
            : m_set(sort(std::vector<Terminal>(begin, end)))
        {
        }

        /**
         * Constructor from null-terminated string.
         * @param str null-terminated string.
         */
        terminal_set_parser(const Terminal* str) noexcept
            : terminal_set_parser(str, str + null_terminated_string_length(str))
        {
        }

        /**
         * Constructor from container.
         * @param set the set of terminal values.
         */
        template <class Container, std::enable_if_t<is_container_v<Container>, bool> = true>
        terminal_set_parser(const Container& set) noexcept
            : terminal_set_parser(set.begin(), set.end())
        {
        }

        /**
         * Constructor from initializer list.
         * @param set the set of terminal values.
         */
        terminal_set_parser(const std::initializer_list<Terminal>& set) noexcept
            : terminal_set_parser(set.begin(), set.end())
        {
        }

        /**
         * Constructor from tuple.
         * @param set the set of terminal values.
         */
        template <class... Terminals>
        terminal_set_parser(const std::tuple<Terminals...>& set) noexcept {
            get_vector<0>(set, m_set);
            m_set = sort(m_set);
        }

        /**
         * Parses a single token against the terminal value set.
         * On success, the parse position is incremented by 1.
         * @param context the parse context.
         * @return true on parse success, false on failure.
         */
        template <class ParseContext>
        bool parse(ParseContext& context) const {
            if (context.is_valid_parse_position()) {
                const auto token = *context.parse_position();
                auto it = std::upper_bound(m_set.begin(), m_set.end(), token, less<typename ParseContext::input_token_comparator_type>());
                if (it != m_set.begin()) {
                    const auto prev = *std::prev(it);
                    if (ParseContext::compare_tokens(token, prev) == 0) {
                        context.increment_parse_position();
                        return true;
                    }
                }
            }
            return false;
        }

        /**
         * Parses a single token against the terminal value set.
         * On success, the parse position is incremented by 1.
         * @param context the parse context.
         * @return true on parse success, false on failure.
         */
        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const {
            return parse(context);
        }

        /**
         * Does nothing, since in the pase left recursion continuation phase,
         * the parse position should not be advanced.
         * @param context the parse context.
         * @param match_start_state parse state that signifies the start of a match.
         * @return false.
         */
        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const {
            return false;
        }

    private:
        std::vector<Terminal> m_set;

        static std::vector<Terminal> sort(const std::vector<Terminal>& vec) noexcept {
            std::vector<Terminal> result(vec.begin(), vec.end());
            std::sort(result.begin(), result.end());
            return result;
        }

        template <std::size_t Index, class... Terminals>
        static void get_vector(const std::tuple<Terminals...>& tpl, std::vector<Terminal>& vec) noexcept {
            if constexpr (Index < sizeof...(Terminals)) {
                vec.push_back(std::get<Index>(tpl));
                get_vector<Index + 1>(tpl, vec);
            }
        }
    };


    /**
     * Helper function for creating a terminal set parser.
     * @param begin beginning of the container that represents the set of values.
     * @param end end of the container that represents the set of values.
     * @return the terminal range parser.
     */
    template <class Iterator>
    terminal_set_parser<typename Iterator::value_type> one_of(const Iterator& begin, const Iterator& end) noexcept {
        return { begin, end };
    }


    /**
     * Helper function for creating a terminal set parser.
     * @param str null-terminated string of characters of the set.
     * @return the terminal range parser.
     */
    template <class Terminal>
    terminal_set_parser<Terminal> one_of(const Terminal* str) noexcept {
        return { str, str + null_terminated_string_length(str)};
    }


    /**
     * Helper function for creating a terminal set parser.
     * @param set the set of terminal values.
     * @return the terminal range parser.
     */
    template <class Container, std::enable_if_t<is_container_v<Container>, bool> = true>
    terminal_set_parser<typename Container::value_type> one_of(const Container& set) noexcept {
        return set;
    }


    /**
     * Helper function for creating a terminal set parser.
     * @param set the set of terminal values.
     * @return the terminal range parser.
     */
    template <class Terminal>
    terminal_set_parser<std::common_type_t<Terminal>> one_of(const std::initializer_list<Terminal>& set) noexcept {
        return set;
    }


    /**
     * Helper function for creating a terminal set parser.
     * @param set the set of terminal values.
     * @return the terminal range parser.
     */
    template <class... Terminals>
    terminal_set_parser<std::common_type_t<Terminals...>> one_of(const std::tuple<Terminals...>& set) noexcept {
        return set;
    }


    /**
     * Helper function for creating a terminal set parser.
     * @param set the set of terminal values.
     * @return the terminal range parser.
     */
    template <class... Terminals>
    terminal_set_parser<std::common_type_t<Terminals...>> one_of(const Terminals&... set) noexcept {
        return std::make_tuple(set...);
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINAL_SET_PARSER_HPP
