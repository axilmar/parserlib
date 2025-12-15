#ifndef PARSERLIB_PARENT_PARSE_NODE_HPP
#define PARSERLIB_PARENT_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * Base class for parse nodes with children.
     * @param Impl type of class that is derived from this class and represents the implementation type.
     * @param Children type of children.
     */
    template <class Impl, class Children> 
    class parent_parse_node : public parse_node<Impl> {
    public:
        /**
         * The constructor.
         * @param children the children.
         */
        parent_parse_node(const Children& children)
            : m_children(children)
        {
        }

        /**
         * Returns the children.
         * @return the children.
         */
        const Children& get_children() const {
            return m_children;
        }

    private:
        Children m_children;
    };


} //namespace parserlib


#endif //PARSERLIB_PARENT_PARSE_NODE_HPP
