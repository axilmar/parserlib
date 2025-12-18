#ifndef PARSERLIB_SEQUENCE_PARSE_NODE_HPP
#define PARSERLIB_SEQUENCE_PARSE_NODE_HPP


#include <type_traits>
#include "tuple_for_each.hpp"
#include "loop_parse_node.hpp"
#include "parse_with_parse_state.hpp"


namespace parserlib {


    struct sequence_parse_node_tag {
    };


    template <class... Children>
    class sequence_parse_node 
        : public parent_parse_node<sequence_parse_node<Children...>, std::tuple<Children...>>
        , public sequence_parse_node_tag
    {
    public:
        using tuple_type = std::tuple<Children...>;

        using parent_type = parent_parse_node<sequence_parse_node<Children...>, tuple_type>;

        sequence_parse_node(const tuple_type& children)
            : parent_type(children)
        {
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return parse_with_parse_state(pc, [&](ParseContext& pc) {
                return tuple_for_each_cond<0, true>(parent_type::get_children(), [&](const auto& child) { 
                    return child.parse(pc); 
                });
            });
        }
    };


    template <class Impl>
    sequence_parse_node<Impl, loop_parse_node<Impl>> parse_node<Impl>::operator +() const {
        return std::make_tuple(*get_impl(), *(*get_impl()));
    }


    template <class L, class R, std::enable_if_t<std::is_base_of_v<parse_node_tag, L> || std::is_base_of_v<parse_node_tag, R>, bool> = true> 
    auto operator >> (const L& left, const R& right) {
        if constexpr (std::is_base_of_v<sequence_parse_node_tag, L> && std::is_base_of_v<sequence_parse_node_tag, R>) {
            return sequence_parse_node(std::tuple_cat(left.get_children(), right.get_children()));
        }
        else if constexpr (std::is_base_of_v<sequence_parse_node_tag, L>) {
            return sequence_parse_node(std::tuple_cat(left.get_children(), std::make_tuple(make_parse_node(right))));
        }
        else if constexpr (std::is_base_of_v<sequence_parse_node_tag, R>) {
            return sequence_parse_node(std::tuple_cat(std::make_tuple(make_parse_node(left)), right.get_children()));
        }
        else {
            return sequence_parse_node(std::make_tuple(make_parse_node(left), make_parse_node(right)));
        }
    }


    template <class L, class R, std::enable_if_t<std::is_base_of_v<parse_node_tag, L> || std::is_base_of_v<parse_node_tag, R>, bool> = true> 
    auto operator - (const L& left, const R& right) {
        return !make_parse_node(right) >> make_parse_node(left);
    }


} //namespace parserlib


#endif //PARSERLIB_SEQUENCE_PARSE_NODE_HPP
