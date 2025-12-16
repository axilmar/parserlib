#ifndef PARSERLIB_LOGICAL_NOT_PARSE_NODE_HPP
#define PARSERLIB_LOGICAL_NOT_PARSE_NODE_HPP


#include "parent_parse_node.hpp"
#include "parse_with_parse_state.hpp"


namespace parserlib {


    /**
     * A parse node that acts as a logical NOT predicate, using another node.
     * @param Child Type of the parse node to make a logical NOT parse node out ot.
     */
    template <class Child>
    class logical_not_parse_node : public parent_parse_node<logical_not_parse_node<Child>, Child> {
    public:
        /** The parent parse node type. */
        using parent_type = parent_parse_node<logical_not_parse_node<Child>, Child>;

        /**
         * The constructor.
         * @param child the child.
         */
        logical_not_parse_node(const Child& child)
            : parent_type(child)
        {
        }

        /**
         * Parses the node; in the end, it restores the parse state to the one
         * before the function is entered.
         * @param pc the context to pass to the child.
         * @return the opposite of what the child parse node returned.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return parse_without_parse_state(pc, [&](ParseContext& pc) { 
                return !parent_type::get_children().parse(pc);
            });
        }
    };


    template <class Impl>
    logical_not_parse_node<Impl> parse_node<Impl>::operator !() const {
        return *get_impl();
    }


} //namespace parserlib


#endif //PARSERLIB_LOGICAL_NOT_PARSE_NODE_HPP
