#ifndef PARSERLIB_CHOICE_PARSE_NODE_HPP
#define PARSERLIB_CHOICE_PARSE_NODE_HPP


#include "multiary_parse_node.hpp"


namespace parserlib {


    struct choice_parse_node_tag {
    };


    template <class... T>
    class choice_parse_node : public multiary_parse_node<choice_parse_node<T...>, T...>, public choice_parse_node_tag {
    public:
        using tuple_type = std::tuple<T...>;

        choice_parse_node(const tuple_type& children)
            : base_class_type(children)
            , m_parse_functions(std::apply([](const auto&... child) { return std::make_tuple(create_parse_function(child)...);  }, this->children()))
        {
        }

        bool parse(parse_context_interface& pc) const {
            return this->_parse<0>(pc);
        }

    private:
        template <class Child>
        static parse_function_type create_parse_function(const Child& child) {
            return [&](parse_context_interface& pc) {
                return child.parse(pc);
            };
        }

        using parse_function_tuple_type = decltype(std::make_tuple(create_parse_function(std::declval<T>())...));

        using base_class_type = multiary_parse_node<choice_parse_node<T...>, T...>;

        const parse_function_tuple_type m_parse_functions;

        template <size_t Index> bool _parse(parse_context_interface& pc) const {
            if constexpr (Index < std::tuple_size_v<tuple_type>) {
                const parse_function_type& parse_function = std::get<Index>(m_parse_functions);
                if (pc.parse(parse_function)) {
                    return true;
                }
                return _parse<Index + 1>(pc);
            }
            else {
                return false;
            }
        }
    };


    template <class L, class R, std::enable_if_t<std::is_base_of_v<parse_node_tag, std::decay_t<L>> || std::is_base_of_v<parse_node_tag, std::decay_t<R>>, bool> = true> 
    auto operator | (L&& left, R&& right) {
        if constexpr (std::is_base_of_v<choice_parse_node_tag, std::decay_t<L>> && std::is_base_of_v<choice_parse_node_tag, std::decay_t<R>>) {
            return choice_parse_node(std::tuple_cat(left.children(), right.children()));
        }
        else if constexpr (std::is_base_of_v<choice_parse_node_tag, std::decay_t<L>>) {
            return choice_parse_node(std::tuple_cat(left.children(), std::make_tuple(make_parse_node(right))));
        }
        else if constexpr (std::is_base_of_v<choice_parse_node_tag, std::decay_t<R>>) {
            return choice_parse_node(std::tuple_cat(std::make_tuple(make_parse_node(left)), right.children()));
        }
        else {
            return choice_parse_node(std::make_tuple(make_parse_node(left), make_parse_node(right)));
        }
    }


} //namespace parserlib


#endif //PARSERLIB_CHOICE_PARSE_NODE_HPP
