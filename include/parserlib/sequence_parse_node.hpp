#ifndef PARSERLIB_SEQUENCE_PARSE_NODE_HPP
#define PARSERLIB_SEQUENCE_PARSE_NODE_HPP


#include "multiary_parse_node.hpp"


namespace parserlib {


    struct sequence_parse_node_tag {
    };


    template <class... T>
    class sequence_parse_node : public multiary_parse_node<sequence_parse_node<T...>, T...>, public sequence_parse_node_tag {
    public:
        using tuple_type = std::tuple<T...>;

        sequence_parse_node(const tuple_type& children)
            : base_class_type(children)
            , m_parse_function([this](parse_context_interface& pc) { 
                return this->_parse<0>(pc);
            })
        {
        }

        bool parse(parse_context_interface& pc) const {
            return pc.parse(m_parse_function);
        }

    private:
        using base_class_type = multiary_parse_node<sequence_parse_node<T...>, T...>;

        const parse_function_type m_parse_function;

        template <size_t Index> bool _parse(parse_context_interface& pc) const {
            if constexpr (Index < std::tuple_size_v<tuple_type>) {
                const auto& child = std::get<Index>(this->children());
                if (child.parse(pc)) {
                    return _parse<Index + 1>(pc);
                }
                return false;
            }
            else {
                return true;
            }
        }
    };


    template <class L, class R, std::enable_if_t<std::is_base_of_v<parse_node_tag, std::decay_t<L>> || std::is_base_of_v<parse_node_tag, std::decay_t<R>>, bool> = true> 
    auto operator >> (L&& left, R&& right) {
        if constexpr (std::is_base_of_v<sequence_parse_node_tag, std::decay_t<L>> && std::is_base_of_v<sequence_parse_node_tag, std::decay_t<R>>) {
            return sequence_parse_node(std::tuple_cat(left.children(), right.children()));
        }
        else if constexpr (std::is_base_of_v<sequence_parse_node_tag, std::decay_t<L>>) {
            return sequence_parse_node(std::tuple_cat(left.children(), std::make_tuple(make_parse_node(right))));
        }
        else if constexpr (std::is_base_of_v<sequence_parse_node_tag, std::decay_t<R>>) {
            return sequence_parse_node(std::tuple_cat(std::make_tuple(make_parse_node(left)), right.children()));
        }
        else {
            return sequence_parse_node(std::make_tuple(make_parse_node(left), make_parse_node(right)));
        }
    }


    template <class L, class R, std::enable_if_t<std::is_base_of_v<parse_node_tag, std::decay_t<L>> || std::is_base_of_v<parse_node_tag, std::decay_t<R>>, bool> = true> 
    auto operator - (L&& left, R&& right) {
        return !make_parse_node(right) >> make_parse_node(left);
    }


} //namespace parserlib


#endif //PARSERLIB_SEQUENCE_PARSE_NODE_HPP
