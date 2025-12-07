#ifndef PARSERLIB_CHOICE_HPP
#define PARSERLIB_CHOICE_HPP


#include <tuple>
#include <type_traits>
#include "make_parse_node.hpp"


namespace parserlib {


    struct choice_tag {};


    template <class... Child>
    class choice : public parse_node<choice<Child...>>, public choice_tag {
    public:
        using tuple_type = std::tuple<Child...>;

        choice(const tuple_type& children) : m_children(children) {
        }

        const tuple_type get_children() const {
            return m_children;
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return pc.parse_choice(m_children);
        }

    private:
        const tuple_type m_children;
    };


    template <class L, class R, std::enable_if_t<std::is_base_of_v<parse_node_tag, std::decay_t<L>> || std::is_base_of_v<parse_node_tag, std::decay_t<R>>, bool> = true>
    auto operator | (L&& left, R&& right) {
        if constexpr (std::is_base_of_v<choice_tag, std::decay_t<L>> && std::is_base_of_v<choice_tag, std::decay_t<R>>) {
            return choice(std::tuple_cat(left.get_children(), right.get_children()));
        }
        else if constexpr (std::is_base_of_v<choice_tag, std::decay_t<L>>) {
            return choice(std::tuple_cat(left.get_children(), std::make_tuple(make_parse_node(right))));
        }
        else if constexpr (std::is_base_of_v<choice_tag, std::decay_t<R>>) {
            return choice(std::tuple_cat(std::make_tuple(make_parse_node(left)), right.get_children()));
        }
        else {
            return choice(std::tuple_cat(std::make_tuple(make_parse_node(left)), std::make_tuple(make_parse_node(right))));
        }
    }


} //namespace parserlib


#endif //PARSERLIB_CHOICE_HPP
