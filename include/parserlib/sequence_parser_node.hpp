#ifndef PARSERLIB_SEQUENCE_PARSER_NODE_HPP
#define PARSERLIB_SEQUENCE_PARSER_NODE_HPP


#include <tuple>
#include "parser_node.hpp"


namespace parserlib {


    struct sequence_parser_node_tag {
    };


    template <class... Parser> class sequence_parser_node : public parser_node<sequence_parser_node<Parser...>>, public sequence_parser_node_tag {
    public:
        sequence_parser_node(const std::tuple<Parser...>& parsers) : m_parsers(parsers) {
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return pc.parse_and_restore_state_on_error([&](ParseContext& pc) { return parse_sequence<0>(pc); });
        }

        const std::tuple<Parser...>& parsers() const {
            return m_parsers;
        }

    private:
        std::tuple<Parser...> m_parsers;

        template <size_t Index, class ParseContext> bool parse_sequence(ParseContext& pc) const {
            if constexpr (Index < std::tuple_size_v<std::tuple<Parser...>>) {
                const auto& parser = std::get<Index>(m_parsers);
                if (parser.parse(pc)) {
                    return parse_sequence<Index + 1>(pc);
                }
                return false;
            }
            else {
                return true;
            }
        }
    };


    template <class L, class R, std::enable_if_t<std::is_base_of_v<parser_node_tag, std::decay_t<L>> || std::is_base_of_v<parser_node_tag, std::decay_t<R>>, bool> = true>
    auto operator >> (L&& left, R&& right) {
        using LT = std::decay_t<L>;
        using RT = std::decay_t<R>;
        if constexpr (std::is_base_of_v<sequence_parser_node_tag, LT> && std::is_base_of_v<sequence_parser_node_tag, RT>) {
            return sequence_parser_node(std::tuple_cat(left.parsers(), right.parsers()));
        }
        else if constexpr (std::is_base_of_v<sequence_parser_node_tag, LT>) {
            return sequence_parser_node(std::tuple_cat(left.parsers(), std::make_tuple(parser(right))));
        }
        else if constexpr (std::is_base_of_v<sequence_parser_node_tag, RT>) {
            return sequence_parser_node(std::tuple_cat(std::make_tuple(parser(left)), right.parsers()));
        }
        else {
            return sequence_parser_node(std::make_tuple(parser(left), parser(right)));
        }
    }


} //namespace parserlib


#endif // PARSERLIB_SEQUENCE_PARSER_NODE_HPP
