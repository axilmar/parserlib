#ifndef PARSERLIB_LOOP_PARSE_NODE_HPP
#define PARSERLIB_LOOP_PARSE_NODE_HPP


#include "parent_parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that makes a loop out of another parse node.
     * @param Child Type of the parse node to create a loop for.
     */
    template <class Child>
    class loop_parse_node : public parent_parse_node<loop_parse_node<Child>, Child> {
    public:
        /** The parent parse node type. */
        using parent_type = parent_parse_node<loop_parse_node<Child>, Child>;

        /**
         * The constructor.
         * @param child the child.
         */
        loop_parse_node(const Child& child)
            : parent_type(child)
        {
        }

        /**
         * Continues parsing while the child returns 'true' from its parse function.
         * @param pc the context to pass to the child.
         * @return true.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            while (parent_type::get_children().parse(pc)) {
            }
            return true;
        }
    };


    template <class Impl>
    loop_parse_node<Impl> parse_node<Impl>::operator *() const {
        return *get_impl();
    }


} //namespace parserlib


#endif //PARSERLIB_LOOP_PARSE_NODE_HPP
