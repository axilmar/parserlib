#ifndef PARSERLIB_OPTIONAL_PARSE_NODE_HPP
#define PARSERLIB_OPTIONAL_PARSE_NODE_HPP


#include "parent_parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that makes another parse node optional.
     * @param Child Type of the parse node to make optional.
     */
    template <class Child>
    class optional_parse_node : public parent_parse_node<optional_parse_node<Child>, Child> {
    public:
        /** The parent parse node type. */
        using parent_type = parent_parse_node<optional_parse_node<Child>, Child>;

        /**
         * The constructor.
         * @param child the child.
         */
        optional_parse_node(const Child& child)
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
            parent_type::get_children().parse(pc);
            return true;
        }
    };


    template <class Impl>
    optional_parse_node<Impl> parse_node<Impl>::operator -() const {
        return *get_impl();
    }


} //namespace parserlib


#endif //PARSERLIB_OPTIONAL_PARSE_NODE_HPP
