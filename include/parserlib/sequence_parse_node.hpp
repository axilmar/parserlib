#ifndef PARSERLIB_SEQUENCE_PARSE_NODE_HPP
#define PARSERLIB_SEQUENCE_PARSE_NODE_HPP


#include <type_traits>
#include "tuple.hpp"
#include "loop_parse_node.hpp"
#include "parse_with_parse_state.hpp"


namespace parserlib {


    template <class... Children>
    class sequence_parse_node : public parent_parse_node<sequence_parse_node<Children...>, std::tuple<Children...>> {
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


    template <class... Children>
    const std::tuple<Children...>& make_parse_node_tuple(const parse_node<sequence_parse_node<Children...>>& sequence) {
        return sequence.get_impl()->get_children();
    }


    template <class L, class R> 
    auto operator >> (const parse_node<L>& left, const parse_node<R>& right) {
        return sequence_parse_node(make_parse_node_tuple(left, right));
    }


    template <class L, class R, std::enable_if_t<!std::is_base_of_v<parse_node_tag, R>, bool> = true> 
    auto operator >> (const parse_node<L>& left, const R& right) {
        return sequence_parse_node(make_parse_node_tuple(left, right));
    }


    template <class L, class R, std::enable_if_t<!std::is_base_of_v<parse_node_tag, L>, bool> = true> 
    auto operator >> (const L& left, const parse_node<R>& right) {
        return sequence_parse_node(make_parse_node_tuple(left, right));
    }


} //namespace parserlib


#endif //PARSERLIB_SEQUENCE_PARSE_NODE_HPP
