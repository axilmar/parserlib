#ifndef PARSERLIB_SEQUENCE_PARSER_NODE_HPP
#define PARSERLIB_SEQUENCE_PARSER_NODE_HPP


#include <tuple>
#include "parser_node.hpp"


namespace parserlib {


    class sequence_parser_node_tag {
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


    template <class L, class R, std::enable_if_t<std::is_base_of_v<parser_node_tag, L> || std::is_base_of_v<parser_node_tag, R>, bool> = true>
    auto operator >> (const L& left, const R& right) {
        if constexpr (std::is_base_of_v<sequence_parser_node_tag, L> && std::is_base_of_v<sequence_parser_node_tag, R>) {
            return sequence_parser_node(std::tuple_cat(left.parsers(), right.parsers()));
        }
        else if constexpr (std::is_base_of_v<sequence_parser_node_tag, L>) {
            return sequence_parser_node(std::tuple_cat(left.parsers(), std::make_tuple(parser(right))));
        }
        else if constexpr (std::is_base_of_v<sequence_parser_node_tag, R>) {
            return sequence_parser_node(std::tuple_cat(std::make_tuple(parser(left)), right.parsers()));
        }
        else {
            return sequence_parser_node(std::make_tuple(parser(left), parser(right)));
        }
    }


} //namespace parserlib


#endif // PARSERLIB_SEQUENCE_PARSER_NODE_HPP
