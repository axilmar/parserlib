#ifndef PARSERLIB_CHOICE_PARSE_NODE_HPP
#define PARSERLIB_CHOICE_PARSE_NODE_HPP


#include <tuple>
#include <type_traits>
#include "parse_node.hpp"


namespace parserlib {


    struct choice_parse_node_tag {
    };


    /**
     * A parse node that contains one or more other parse nodes,
     * which one of them must parse successfully in order
     * for this node to parse successfully.
     * @param Children types of children parse nodes.
     */
    template <class ...Children>
    class choice_parse_node 
        : public parse_node<choice_parse_node<Children...>>
        , public choice_parse_node_tag
    {
    public:
        /** Type of tuple for children. */
        using tuple_type = std::tuple<Children...>;

        /**
         * The constructor.
         * @param children the children.
         */
        choice_parse_node(const tuple_type& children)
            : m_children(children)
        {
        }

        /**
         * Invokes one child parse node after the other.
         * If a child parses successfully, then it returns true.
         * Otherwise, the parse context state is restored to the state
         * and the next child is invoked.
         * @param pc the parse context.
         * @return true on success, false on failure.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return _parse<0>(pc);
        }

        const tuple_type& get_children() const {
            return m_children;
        }

    private:
        tuple_type m_children;

        template <class ParseContext, class Child>
        static bool _parse_child(ParseContext& pc, const Child& child) {
            return pc.parse_and_restore_state_on_failure([&]() {
                return child.parse(pc);
            });
        }

        template <size_t Index, class ParseContext>
        bool _parse(ParseContext& pc) const {
            if constexpr (Index < std::tuple_size_v<tuple_type>) {
                const auto& child = std::get<Index>(m_children);
                if (_parse_child(pc, child)) {
                    return true;
                }
                return _parse<Index + 1>(pc);
            }
            else {
                return false;
            }
        }
    };


    /**
     * An operator that creates a choice.
     * One of the two operands must be a parse node;
     * if an operand is not a parse node, then the function 'make_parse_node'
     * is invoked on it, to convert it to a parse node.
     * Choices are flattened.
     * @param left the left operand.
     * @param right the right operand.
     * @return the choice of the two operands.
     */
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
