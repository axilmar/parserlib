#ifndef PARSERLIB_CHOICE_PARSE_NODE_HPP
#define PARSERLIB_CHOICE_PARSE_NODE_HPP


#include <tuple>
#include <type_traits>
#include <sstream>
#include "parse_node.hpp"
#ifndef NDEBUG
#include "tuple.hpp"
#endif

namespace parserlib {


    /**
     * Base class for choice parse nodes.
     */
    class choice_parse_node_base {
    };


    /**
     * A parse node that invokes a series of children parse nodes to parse.
     * At least one of the children must parse successfully in order for this parse node to parse successfully.
     * @param ParseNodes list of children parse node types.
     */
    template <class... ParseNodes>
    class choice_parse_node : public parse_node<choice_parse_node<ParseNodes...>>, public choice_parse_node_base {
    public:
        /**
         * The constructor.
         * @param children the children parse nodes.
         */
        choice_parse_node(const std::tuple<ParseNodes...>& children)
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
            return _parse<0>(pc);
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
                    stream << " | ";
                }
                stream << child.text();
                ++count;
            });
            stream << ')';
            return stream.str();
        }

    private:
        const std::tuple<ParseNodes...> m_children;

        template <class ParseNode, class ParseContext> 
        bool invoke_child(const ParseNode& child, ParseContext& pc) const {
            const auto state = pc.get_state();
            if (pc.parse(child)) {
                return true;
            }
            pc.set_state(state);
            return false;
        }

        template <size_t Index, class ParseContext>
        bool _parse(ParseContext& pc) const {
            if constexpr (Index < std::tuple_size_v<std::tuple<ParseNodes...>>) {
                const auto& child = std::get<Index>(m_children);
                if (invoke_child(child, pc)) {
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
     * Operator that creates a choice out of one or two parse nodes.
     * Instances of choice_parse_node are flatted into the result node.
     * @param left the left operand.
     * @param right the right operand.
     * @return the choice of the given operands.
     */
    template <
        class L,
        class R,
        std::enable_if_t<std::is_base_of_v<parse_node_base, std::decay_t<L>> || std::is_base_of_v<parse_node_base, std::decay_t<R>>, bool> = true
    >
    auto operator | (L&& left, R&& right) {
        if constexpr (std::is_base_of_v<choice_parse_node_base, std::decay_t<L>> && std::is_base_of_v<choice_parse_node_base, std::decay_t<R>>) {
            return choice_parse_node(std::tuple_cat(left.children(), right.children()));
        }
        else if constexpr (std::is_base_of_v<choice_parse_node_base, std::decay_t<L>>) {
            return choice_parse_node(std::tuple_cat(left.children(), std::make_tuple(make_parse_node(right))));
        }
        else if constexpr (std::is_base_of_v<choice_parse_node_base, std::decay_t<R>>) {
            return choice_parse_node(std::tuple_cat(std::make_tuple(make_parse_node(left)), right.children()));
        }
        else {
            return choice_parse_node(std::make_tuple(make_parse_node(left), make_parse_node(right)));
        }
    }


} //namespace parserlib


#endif //PARSERLIB_CHOICE_PARSE_NODE_HPP
