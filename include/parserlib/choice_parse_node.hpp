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
            const auto base_error_state = pc.get_error_state();
            typename ParseContext::parse_error_container_type child_errors;
            return _parse<0>(pc, base_error_state, child_errors);
        }

        const tuple_type& get_children() const {
            return m_children;
        }

    private:
        tuple_type m_children;

        //if the child creates some errors, keep the errors that are further into the source
        template <class ParseContext, class Child, class ErrorState, class ErrorContainer>
        static bool _parse_child(ParseContext& pc, const Child& child, const ErrorState& base_error_state, ErrorContainer& child_errors) {
            return parse_and_restore_state_on_failure(pc, [&]() {
                const auto base_iterator = pc.get_iterator();

                //parse
                const bool result = child.parse(pc);

                //on success
                if (result) {
                    return true;
                }

                //handle new errors
                if (pc.get_errors().size() > base_error_state.get_error_count()) {
                    const auto& errors = pc.get_errors();

                    //if there were no child errors previously, then store all the new errors as child errors
                    if (child_errors.empty()) {
                        child_errors.insert(child_errors.end(), errors.begin() + base_error_state.get_error_count(), errors.end());
                    }

                    //otherwise compare the last error created by the current child
                    //and the last error created by previous child;
                    //keep the errors with the furthest position
                    else {
                        const auto distance_of_current_child_last_error_begin = std::distance(base_iterator, errors.back().begin());
                        const auto distance_of_last_child_last_error_begin = std::distance(base_iterator, child_errors.back().begin());

                        //if current child last error begin is further down the last child last error begin
                        if (distance_of_current_child_last_error_begin > distance_of_last_child_last_error_begin) {
                            child_errors.clear();
                            child_errors.insert(child_errors.end(), errors.begin() + base_error_state.get_error_count(), errors.end());
                        }

                        //else if there are at the same position
                        else if (distance_of_current_child_last_error_begin == distance_of_last_child_last_error_begin) {
                            const auto distance_of_current_child_last_error_end = std::distance(base_iterator, errors.back().end());
                            const auto distance_of_last_child_last_error_end = std::distance(base_iterator, child_errors.back().end());

                            //if current child last error end is further down the last child last error end
                            if (distance_of_current_child_last_error_end > distance_of_last_child_last_error_end) {
                                child_errors.clear();
                                child_errors.insert(child_errors.end(), errors.begin() + base_error_state.get_error_count(), errors.end());
                            }
                        }
                    }

                    //reset the error state for the next child
                    pc.set_error_state(base_error_state);
                }

                return false;
            });
        }

        template <size_t Index, class ParseContext, class ErrorState, class ErrorContainer>
        bool _parse(ParseContext& pc, const ErrorState& base_error_state, ErrorContainer& child_errors) const {
            if constexpr (Index < parserlib::tuple_size_v<tuple_type>) {
                const auto& child = parserlib::get<Index>(m_children);
                if (_parse_child(pc, child, base_error_state, child_errors)) {
                    return true;
                }
                return _parse<Index + 1>(pc, base_error_state, child_errors);
            }
            else {
                pc.add_errors(child_errors);
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
