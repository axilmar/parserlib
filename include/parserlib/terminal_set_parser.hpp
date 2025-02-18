#ifndef PARSERLIB_TERMINAL_SET_PARSER_HPP
#define PARSERLIB_TERMINAL_SET_PARSER_HPP


#include <vector>
#include <set>
#include <initializer_list>
#include <algorithm>
#include "parser.hpp"
#include "less.hpp"


namespace parserlib {


    template <class Terminal>
    class terminal_set_parser : public parser<terminal_set_parser<Terminal>> {
    public:
        terminal_set_parser(const std::vector<Terminal>& set) noexcept
            : m_set(sorted(set))
        {
        }

        template <class Iterator>
        terminal_set_parser(const Iterator& begin, const Iterator& end) noexcept
            : m_set(sorted(std::vector<Terminal>(begin, end)))
        {
        }

        template <class Pr, class Alloc>
        terminal_set_parser(const std::set<Terminal, Pr, Alloc>& set) noexcept
            : m_set(set.begin(), set.end())
        {
        }

        terminal_set_parser(const std::initializer_list<Terminal>& set) noexcept
            : m_set(sorted(std::vector<Terminal>(set.begin(), set.end())))
        {
        }

        template <class ParseContext>
        bool parse(ParseContext& context) const noexcept {
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

        template <class ParseContext>
        bool parse_left_recursion_start(ParseContext& context) const noexcept {
            return parse(context);
        }

        template <class ParseContext>
        bool parse_left_recursion_continuation(ParseContext& context, const typename ParseContext::state& match_start_state) const noexcept {
            return false;
        }

    private:
        const std::vector<Terminal> m_set;

        static std::vector<Terminal> sorted(const std::vector<Terminal>& vec) noexcept {
            std::vector<Terminal> result(vec.begin(), vec.end());
            std::sort(result.begin(), result.end());
            return result;
        }
    };


    template <class Terminal>
    terminal_set_parser<Terminal> terminal(const std::vector<Terminal>& set) noexcept {
        return set;
    }


    template <class Iterator>
    terminal_set_parser<typename Iterator::value_type> terminal(const Iterator& begin, const Iterator& end) noexcept {
        return { begin, end };
    }


    template <class Terminal, class Pr, class Alloc>
    terminal_set_parser<Terminal> terminal(const std::set<Terminal, Pr, Alloc>& set) noexcept {
        return set;
    }


    template <class Terminal>
    terminal_set_parser<Terminal> terminal(const std::initializer_list<Terminal>& set) noexcept {
        return set;
    }


    template <class Terminal>
    terminal_set_parser<Terminal> one_of(const std::vector<Terminal>& set) noexcept {
        return set;
    }


    template <class Iterator>
    terminal_set_parser<typename Iterator::value_type> one_of(const Iterator& begin, const Iterator& end) noexcept {
        return { begin, end };
    }


    template <class Terminal, class Pr, class Alloc>
    terminal_set_parser<Terminal> one_of(const std::set<Terminal, Pr, Alloc>& set) noexcept {
        return set;
    }


    template <class Terminal>
    terminal_set_parser<Terminal> one_of(const std::initializer_list<Terminal>& set) noexcept {
        return set;
    }


} //namespace parserlib


#endif //PARSERLIB_TERMINAL_SET_PARSER_HPP
