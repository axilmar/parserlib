#ifndef PARSERLIB_CHOICE_PARSE_NODE_HPP
#define PARSERLIB_CHOICE_PARSE_NODE_HPP


#include <type_traits>
#include "tuple.hpp"
#include "parse_node.hpp"
#include "parse_algorithms.hpp"


namespace parserlib {


    struct choice_parse_node_tag {
    };


    /**
     * A parse node that contains one or more other parse nodes,
     * which one of them must parse successfully in order
     * for this node to parse successfully.
     * @param Tpl type of tuple for the children.
     */
    template <class Tpl>
    class choice_parse_node 
        : public parse_node<choice_parse_node<Tpl>>
        , public choice_parse_node_tag
    {
    public:
        /** Type of tuple for children. */
        using tuple_type = Tpl;

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
            return parse_and_restore_state_and_error_state_on_failure(pc, [&]() {
                return child.parse(pc);
            });
        }

        template <size_t Index, class ParseContext>
        bool _parse(ParseContext& pc) const {
            if constexpr (Index < parserlib::tuple_size_v<tuple_type>) {
                const auto& child = parserlib::get<Index>(m_children);
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
            return choice_parse_node(parserlib::tuple_cat(left.get_children(), right.get_children()));
        }
        else if constexpr (std::is_base_of_v<choice_parse_node_tag, L>) {
            return choice_parse_node(parserlib::tuple_cat(left.get_children(), parserlib::make_tuple(make_parse_node(right))));
        }
        else if constexpr (std::is_base_of_v<choice_parse_node_tag, R>) {
            return choice_parse_node(parserlib::tuple_cat(parserlib::make_tuple(make_parse_node(left)), right.get_children()));
        }
        else {
            return choice_parse_node(parserlib::make_tuple(make_parse_node(left), make_parse_node(right)));
        }
    }


} //namespace parserlib


#endif //PARSERLIB_CHOICE_PARSE_NODE_HPP
