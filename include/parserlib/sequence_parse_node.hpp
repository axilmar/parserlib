#ifndef PARSERLIB_SEQUENCE_PARSE_NODE_HPP
#define PARSERLIB_SEQUENCE_PARSE_NODE_HPP


#include <tuple>
#include <type_traits>
#include "parse_node.hpp"
#ifndef NDEBUG
#include "tuple.hpp"
#endif


namespace parserlib {


    /**
     * Base class for sequence parse nodes.
     */
    class sequence_parse_node_base {
    };


    /**
     * A parse node that invokes a series of children parse nodes to parse.
     * All the children must parse successfully in order for this parse node to parse successfully.
     * @param ParseNodes list of children parse node types.
     */
    template <class... ParseNodes> 
    class sequence_parse_node : public parse_node<sequence_parse_node<ParseNodes...>>, public sequence_parse_node_base {
    public:
        /**
         * The constructor.
         * @param children the children parse nodes.
         */
        sequence_parse_node(const std::tuple<ParseNodes...>& children) 
            : m_children(children)
        {
        }

        /**
         * Invokes one child node after the other.
         * @param pc the current parse context; if parsing fails, then the state of the parse context on return
         *  is exactly the same as the state of the parse context on entrance.
         * @return false if at least one child parse node returns false, otherwise it returns true.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            const auto start_state = pc.get_state();
            if (_parse<0>(pc)) {
                return true;
            }
            pc.set_state(start_state);
            return false;
        }

        /**
         * Returns the children nodes.
         * @return the children nodes.
         */
        const std::tuple<ParseNodes...>& children() const {
            return m_children;
        }

        /**
         * Converts the parse node to a textual description.
         * @return a string of this parse node as text.
         */
        std::string text() const override {
            std::stringstream stream;
            size_t count = 0;
            stream << '(';
            tuple_for_each(m_children, [&](const auto& child) {
                if (count) {
                    stream << " >> ";
                }
                stream << child.text();
                ++count;
            });
            stream << ')';
            return (stream.str());
        }

    private:
        const std::tuple<ParseNodes...> m_children;

        template <size_t Index, class ParseContext>
        bool _parse(ParseContext& pc) const {
            if constexpr (Index < std::tuple_size_v<std::tuple<ParseNodes...>>) {
                const auto& child = std::get<Index>(m_children);
                if (pc.parse(child)) {
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
     * Operator that creates a sequence out of one or two parse nodes.
     * Instances of sequence_parse_node are flatted into the result node.
     * @param left the left operand.
     * @param right the right operand.
     * @return the sequence of the given operands.
     */
    template <
        class L, 
        class R, 
        std::enable_if_t<std::is_base_of_v<parse_node_base, std::decay_t<L>> || std::is_base_of_v<parse_node_base, std::decay_t<R>>, bool> = true
    >
    auto operator >> (L&& left, R&& right) {
        if constexpr (std::is_base_of_v<sequence_parse_node_base, std::decay_t<L>> && std::is_base_of_v<sequence_parse_node_base, std::decay_t<R>>) {
            return sequence_parse_node(std::tuple_cat(left.children(), right.children()));
        }
        else if constexpr (std::is_base_of_v<sequence_parse_node_base, std::decay_t<L>>) {
            return sequence_parse_node(std::tuple_cat(left.children(), std::make_tuple(make_parse_node(right))));
        }
        else if constexpr (std::is_base_of_v<sequence_parse_node_base, std::decay_t<R>>) {
            return sequence_parse_node(std::tuple_cat(std::make_tuple(make_parse_node(left)), right.children()));
        }
        else {
            return sequence_parse_node(std::make_tuple(make_parse_node(left), make_parse_node(right)));
        }
    }


    /**
     * The exclusion operator.
     * Converts an expression of type "L - R" to "!R >> L".
     * @param left left operand.
     * @param right right operand.
     * @return a sequence of not R followed by L.
     */
    template <
        class L,
        class R,
        std::enable_if_t<std::is_base_of_v<parse_node_base, std::decay_t<L>> || std::is_base_of_v<parse_node_base, std::decay_t<R>>, bool> = true
    >
    auto operator - (L&& left, R&& right) {
        return !make_parse_node(right) >> make_parse_node(left);
    }


} //namespace parserlib


#endif //PARSERLIB_SEQUENCE_PARSE_NODE_HPP
