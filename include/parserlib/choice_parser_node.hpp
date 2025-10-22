#ifndef PARSERLIB_CHOICE_PARSER_NODE_HPP
#define PARSERLIB_CHOICE_PARSER_NODE_HPP


#include <tuple>
#include "parser_node.hpp"


namespace parserlib {


    struct choice_parser_node_tag {
    };


    template <class... Parser> class choice_parser_node : public parser_node<choice_parser_node<Parser...>>, public choice_parser_node_tag {
    public:
        choice_parser_node(const std::tuple<Parser...>& parsers) : m_parsers(parsers) {
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return parse_choice<0>(pc);
        }

        const std::tuple<Parser...>& parsers() const {
            return m_parsers;
        }

    private:
        std::tuple<Parser...> m_parsers;

        template <size_t Index, class ParseContext> bool parse_choice(ParseContext& pc) const {
            if constexpr (Index < std::tuple_size_v<std::tuple<Parser...>>) {
                const auto& parser = std::get<Index>(m_parsers);
                if (pc.parse_and_restore_state_on_error([&](ParseContext& pc){ return parser.parse(pc); })) {
                    return true;
                }
                return parse_choice<Index + 1>(pc);
            }
            else {
                return false;
            }
        }
    };


    template <class L, class R, std::enable_if_t<std::is_base_of_v<parser_node_tag, std::decay_t<L>> || std::is_base_of_v<parser_node_tag, std::decay_t<R>>, bool> = true>
    auto operator | (L&& left, R&& right) {
        using LT = std::decay_t<L>;
        using RT = std::decay_t<R>;
        if constexpr (std::is_base_of_v<choice_parser_node_tag, LT> && std::is_base_of_v<choice_parser_node_tag, RT>) {
            return choice_parser_node(std::tuple_cat(left.parsers(), right.parsers()));
        }
        else if constexpr (std::is_base_of_v<choice_parser_node_tag, LT>) {
            return choice_parser_node(std::tuple_cat(left.parsers(), std::make_tuple(parser(right))));
        }
        else if constexpr (std::is_base_of_v<choice_parser_node_tag, RT>) {
            return choice_parser_node(std::tuple_cat(std::make_tuple(parser(left)), right.parsers()));
        }
        else {
            return choice_parser_node(std::make_tuple(parser(left), parser(right)));
        }
    }


} //namespace parserlib


#endif // PARSERLIB_CHOICE_PARSER_NODE_HPP
