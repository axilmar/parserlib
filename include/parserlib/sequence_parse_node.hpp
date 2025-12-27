#ifndef PARSERLIB_SEQUENCE_PARSE_NODE_HPP
#define PARSERLIB_SEQUENCE_PARSE_NODE_HPP


#include <tuple>
#include <type_traits>
#include "parse_node.hpp"
#include "parse_context.hpp"
#include "parse_algorithms.hpp"


namespace parserlib {


    struct sequence_parse_node_tag {
    };


    /**
     * A parse node that contains one or more other parse nodes,
     * which must all parse successfully in sequence
     * for this node to parse successfully.
     * @param Children types of children parse nodes.
     */
    template <class ...Children>
    class sequence_parse_node 
        : public parse_node<sequence_parse_node<Children...>>
        , public sequence_parse_node_tag
    {
    public:
        /** Type of tuple for children. */
        using tuple_type = std::tuple<Children...>;

        /**
         * The constructor.
         * @param children the children.
         */
        sequence_parse_node(const tuple_type& children)
            : m_children(children)
        {
        }

        /**
         * Invokes one child parse node after the other.
         * If all children parse successfully, then it returns true.
         * Otherwise, the parse context state is restored to the state
         * before this call and false is returned.
         * @param pc the parse context.
         * @return true on success, false on failure.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return parse_and_restore_state_on_failure(pc, [&]() {
                return _parse<0>(pc);
            });
        }

        const tuple_type& get_children() const {
            return m_children;
        }

    private:
        tuple_type m_children;

        template <size_t Index, class ParseContext>
        bool _parse(ParseContext& pc) const {
            if constexpr (Index < std::tuple_size_v<tuple_type>) {
                const auto& child = std::get<Index>(m_children);
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


    /**
     * An operator that creates a sequence.
     * One of the two operands must be a parse node;
     * if an operand is not a parse node, then the function 'make_parse_node'
     * is invoked on it, to convert it to a parse node.
     * Sequences are flattened.
     * @param left the left operand.
     * @param right the right operand.
     * @return the sequence of the two operands.
     */
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


    /**
     * The exclusion operator.
     * It converts a grammar of the form `A - B` to `!B >> A`.
     * @param left the left operand.
     * @param right the right operand.
     * @return the sequence of the logical negation of the right argument and the left argument.
     */
    template <class L, class R, std::enable_if_t<std::is_base_of_v<parse_node_tag, L> || std::is_base_of_v<parse_node_tag, R>, bool> = true>
    auto operator - (const L& left, const R& right) {
        return !make_parse_node(right) >> make_parse_node(left);
    }


} //namespace parserlib


#endif //PARSERLIB_SEQUENCE_PARSE_NODE_HPP
