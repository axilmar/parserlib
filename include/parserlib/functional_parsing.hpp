#ifndef PARSERLIB_FUNCTIONAL_PARSING_HPP
#define PARSERLIB_FUNCTIONAL_PARSING_HPP


#include <tuple>
#include <cstring>
#include <string>
#include <set>
#include <vector>
#include <type_traits>
#include <string_view>


namespace parserlib::functional {


    struct parse_terminal_traits {
        template <typename T1, typename T2>
        static bool equal_to(const T1& t1, const T2& t2) {
            return t1 == t2;
        }

        template <typename T1, typename T2>
        static bool different_than(const T1& t1, const T2& t2) {
            return !equal_to(t1, t2);
        }

        template <typename T1, typename T2>
        static bool less_than(const T1& t1, const T2& t2) {
            return t1 < t2;
        }

        template <typename T1, typename T2>
        static bool less_than_or_equal_to(const T1& t1, const T2& t2) {
            return t1 <= t2;
        }

        template <typename T1, typename T2>
        static bool greater_than(const T1& t1, const T2& t2) {
            return t1 > t2;
        }

        template <typename T1, typename T2>
        static bool greater_than_or_equal_to(const T1& t1, const T2& t2) {
            return t1 >= t2;
        }

        struct LessThan {
            template <typename T1, typename T2>
            bool operator ()(const T1& t1, const T2& t2) const {
                return less_than(t1, t2);
            }
        };
    };


    template <typename SourceType, typename MatchIdType>
    struct match {
        using iterator_type = typename SourceType::const_iterator;
        using value_type = typename SourceType::value_type;

        MatchIdType id;
        iterator_type start_position;
        iterator_type end_position;
        std::vector<match<SourceType, MatchIdType>> children;

        auto get_source() const {
            if constexpr (std::is_trivial_v<value_type>) {
                return std::basic_string_view<value_type>(&*start_position, end_position - start_position);
            }
            else {
                return std::vector<value_type>(start_position, end_position);
            }
        }
    };


    template <typename SourceType, typename MatchIdType, typename TerminalTraitsType = parse_terminal_traits> 
    struct parse_context : TerminalTraitsType {
        using source_type = SourceType;

        using TokenType = typename SourceType::value_type;

        using iterator_type = typename SourceType::const_iterator;

        struct state {
            iterator_type parse_position;
            size_t matches_size;
        };

        iterator_type parse_position;

        const iterator_type end_position;

        std::vector<match<SourceType, MatchIdType>> matches;

        parse_context(SourceType& src)
            : parse_position(src.begin())
            , end_position(src.end())
        {
        }

        state get_state() const {
            return { parse_position, matches.size() };
        }

        void set_state(const state& s) {
            parse_position = s.parse_position;
            matches.resize(s.matches_size);
        }

        void add_match(const MatchIdType& id, const state& start, const state& end) {
            const auto match_start = matches.begin() + start.matches_size;
            const auto match_end = matches.begin() + end.matches_size;
            std::vector<match<SourceType, MatchIdType>> children{ match_start, match_end };
            matches.erase(match_start, match_end);
            matches.push_back({ id, start.parse_position, end.parse_position, std::move(children) });
        }
    };


    template <typename SourceType, typename MatchIdType>
    struct parse_result {
        using iterator_type = typename SourceType::const_iterator;
        bool success;
        std::vector<match<SourceType, MatchIdType>> matches;
        iterator_type parse_position;
        bool parse_success;
        bool completion_success;
    };


    template <typename ContextType, typename TerminalType> 
    bool parse_terminal(ContextType& context, const TerminalType& terminal) {
        if (context.parse_position != context.end_position && ContextType::equal_to(*context.parse_position, terminal)) {
            ++context.parse_position;
            return true;
        }
        return false;
    }


    template <typename ContextType, typename TerminalType>
    bool parse_terminal(ContextType& context, const TerminalType* begin, const TerminalType* end) {
        auto it = context.parse_position;
        while (true) {
            if (begin == end) {
                context.parse_position = it;
                return true;
            }
            if (it == context.end_position) {
                break;
            }
            if (!ContextType::equal_to(*it, *begin)) {
                break;
            }
            ++it;
            ++begin;
        }
        return false;
    }


    template <typename ContextType, typename TerminalType>
    bool parse_terminal(ContextType& context, const TerminalType* str) {
        return parse_terminal(context, str, str + strlen(str));
    }


    template <typename ContextType, typename TerminalType, typename AllocType>
    bool parse_terminal(ContextType& context, const std::basic_string<TerminalType, AllocType>& str) {
        return parse_terminal(context, str.begin(), str.end());
    }


    template <typename ContextType, typename TerminalType, typename AllocType>
    bool parse_terminal(ContextType& context, const std::set<TerminalType, std::less<TerminalType>, AllocType>& set) {
        if (context.parse_position != context.end_position) {
            const auto& remapped_set = (const std::set<TerminalType, typename ContextType::LessThan, AllocType>&)set;
            if (remapped_set.find(*context.parse_position) != remapped_set.end()) {
                ++context.parse_position;
                return true;
        }
        }
        return false;
    }


    template <typename ContextType, typename TerminalType>
    bool parse_terminal(ContextType& context, const TerminalType& minTerminal, const TerminalType& maxTerminal) {
        if (context.parse_position != context.end_position && 
            ContextType::less_than_or_equal_to(minTerminal, *context.parse_position) && 
            ContextType::less_than_or_equal_to(*context.parse_position, maxTerminal))
        {
            ++context.parse_position;
            return true;
        }
        return false;
    }


    template <typename ContextType, typename MatchStartType, typename FuncType>
    bool parse_logical_and(ContextType& context, const MatchStartType& matchStart, const FuncType& func) {
        auto saved_state = context.get_state();
        const bool result = func(context, matchStart);
        context.set_state(saved_state);
        return result;
    }


    template <typename ContextType, typename MatchStartType, typename FuncType>
    bool parse_logical_not(ContextType& context, const MatchStartType& matchStart, const FuncType& func) {
        auto saved_state = context.get_state();
        const bool result = !func(context, matchStart);
        context.set_state(saved_state);
        return result;
    }


    template <typename ContextType, typename MatchStartType, typename FuncType>
    bool parse_zero_or_more_times(ContextType& context, const MatchStartType& matchStart, const FuncType& func) {
        if (func(context, matchStart)) {
            while (true) {
                if (func(context, context.get_state())) {
                    continue;
                }
                break;
            }
        }
        return true;
    }


    template <typename ContextType, typename MatchStartType, typename FuncType>
    bool parse_one_or_more_times(ContextType& context, const MatchStartType& matchStart, const FuncType& func) {
        if (!func(context, matchStart)) {
            return false;
        }
        while (true) {
            if (func(context, context.get_state())) {
                continue;
            }
            break;
        }
        return true;
    }


    template <typename ContextType, typename MatchStartType, typename FuncType>
    bool parse_optional(ContextType& context, const MatchStartType& matchStart, const FuncType& func) {
        func(context, matchStart);
        return true;
    }


    template <typename ContextType, typename MatchStartType, typename FuncType, typename... TailFuncType>
    bool parse_sequence_helper(ContextType& context, const MatchStartType& matchStart, const FuncType& func, const TailFuncType&... funcs) {
        if (func(context, matchStart)) {
            return parse_sequence_helper(context, context.get_state(), funcs...);
        }
        return false;
    }


    template <typename ContextType, typename MatchStartType, typename FuncType>
    bool parse_sequence_helper(ContextType& context, const MatchStartType& matchStart, const FuncType& func) {
        return func(context, matchStart);
    }


    template <typename ContextType, typename MatchStartType, typename... FuncType>
    bool parse_sequence(ContextType& context, const MatchStartType& matchStart, const FuncType&... funcs) {
        const auto saved_state = context.get_state();
        if (parse_sequence_helper(context, matchStart, funcs...)) {
            return true;
        }
        context.set_state(saved_state);
        return false;
    }


    template <typename ContextType, typename MatchStartType, typename FuncType, typename... TailFuncType>
    bool parse_choice(ContextType& context, const MatchStartType& matchStart, const FuncType& func, const TailFuncType&... funcs) {
        const auto saved_state = context.get_state();
        if (func(context, matchStart)) {
            return true;
        }
        context.set_state(saved_state);
        return parse_choice(context, matchStart, funcs...);
    }


    template <typename ContextType, typename MatchStartType, typename FuncType>
    bool parse_choice(ContextType& context, const MatchStartType& matchStart, const FuncType& func) {
        const auto saved_state = context.get_state();
        if (func(context, matchStart)) {
            return true;
        }
        context.set_state(saved_state);
        return false;
    }


    template <typename ContextType, typename MatchStartType, typename FuncType, typename MatchIdType>
    bool parse_match(ContextType& context, const MatchStartType& matchStart, const MatchIdType& matchId, const FuncType& func) {
        if (func(context, matchStart)) {
            context.add_match(matchId, matchStart, context.get_state());
            return true;
        }
        return false;
    }


    template <typename ContextType, typename MatchStartType, typename BaseFuncType, typename... BranchFuncType>
    bool parse_left_recursion(ContextType& context, const MatchStartType& matchStart, const BaseFuncType& base, const BranchFuncType&... branches) {
        if (base(context, matchStart)) {
            while (true) {
                if (parse_choice(context, matchStart, branches...)) {
                    continue;
                }
                break;
            }
            return true;
        }
        return false;
    }

    template <typename MatchIdType, typename SourceType, typename FuncType, typename TerminalTraitsType>
    parse_result<SourceType, MatchIdType> parse(SourceType& src, const FuncType& func, TerminalTraitsType terminalTraits) {
        parse_context<SourceType, MatchIdType, TerminalTraitsType> context(src);
        const bool parse_success = func(context, context.get_state());
        const bool completion_success = context.parse_position == context.end_position;
        const bool success = parse_success && completion_success;
        return { success, std::move(context.matches), context.parse_position, parse_success, completion_success };
    }


    template <typename MatchIdType, typename SourceType, typename FuncType>
    parse_result<SourceType, MatchIdType> parse(SourceType& src, const FuncType& func) {
        return parse<MatchIdType>(src, func, parse_terminal_traits());
    }


} //namespace parserlib::functional


#endif //PARSERLIB_FUNCTIONAL_PARSING_HPP
