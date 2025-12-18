#ifndef PARSERLIB_CHOICE_PARSE_NODE_HPP
#define PARSERLIB_CHOICE_PARSE_NODE_HPP


#include <type_traits>
#include "tuple_for_each.hpp"
#include "loop_parse_node.hpp"
#include "parse_with_parse_state.hpp"


namespace parserlib {


    struct choice_parse_node_tag {};


    template <class... Children>
    class choice_parse_node 
        : public parent_parse_node<choice_parse_node<Children...>, std::tuple<Children...>>
        , public choice_parse_node_tag
    {
    public:
        using tuple_type = std::tuple<Children...>;

        using parent_type = parent_parse_node<choice_parse_node<Children...>, tuple_type>;

        choice_parse_node(const tuple_type& children)
            : parent_type(children)
        {
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return parse_with_parse_state(pc, [&](ParseContext& pc) {
                return tuple_for_each_cond<0, false>(parent_type::get_children(), [&](const auto& child) { 
                    return child.parse(pc); 
                });
            });
        }
    };


    template <class L, class R, std::enable_if_t<std::is_base_of_v<parse_node_tag, L> || std::is_base_of_v<parse_node_tag, R>, bool> = true> 
    auto operator | (const L& left, const R& right) {
        if constexpr (std::is_base_of_v<choice_parse_node_tag, L> && std::is_base_of_v<choice_parse_node_tag, R>) {
            return choice_parse_node(std::tuple_cat(left.get_children(), right.get_children()));
        }
        else if constexpr (std::is_base_of_v<choice_parse_node_tag, L>) {
            return choice_parse_node(std::tuple_cat(left.get_children(), std::make_tuple(make_parse_node(right))));
        }
        else if constexpr (std::is_base_of_v<choice_parse_node_tag, R>) {
            return choice_parse_node(std::tuple_cat(std::make_tuple(make_parse_node(left)), right.get_children()));
        }
        else {
            return choice_parse_node(std::make_tuple(make_parse_node(left), make_parse_node(right)));
        }
    }


} //namespace parserlib


#endif //PARSERLIB_CHOICE_PARSE_NODE_HPP
