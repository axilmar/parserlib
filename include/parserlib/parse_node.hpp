#ifndef PARSERLIB_PARSE_NODE_HPP
#define PARSERLIB_PARSE_NODE_HPP


namespace parserlib {


    template <class Child> class loop_parse_node;
    template <class Child> class optional_parse_node;
    template <class Child> class logical_and_parse_node;
    template <class Child> class logical_not_parse_node;
    template <class... Children> class sequence_parse_node;
    template <class ParseContext> class parse_node_ptr;


    struct parse_node_tag {
    };


    /**
     * Base class for parse nodes.
     * Also provides the unary operators for parse nodes.
     * @param Impl type of class that is derived from this class and represents the implementation type.
     */
    template <class Impl>
    class parse_node : public parse_node_tag {
    public:
        /**
         * Returns pointer to implementation for this parse node.
         * @return pointer to implementation for this parse node.
         */
        const Impl* get_impl() const {
            return static_cast<const Impl*>(this);
        }

        /**
         * Returns pointer to implementation for this parse node.
         * @return pointer to implementation for this parse node.
         */
        Impl* get_impl() {
            return static_cast<Impl*>(this);
        }

        /**
         * Operator that converts a parse node into a loop.
         * @return a loop parse node.
         */
        loop_parse_node<Impl> operator *() const;

        /**
         * Operator that converts a parse node into a loop 
         * that must parse successfully at least once.
         * @return a sequence of this and a loop of this.
         */
        sequence_parse_node<Impl, loop_parse_node<Impl>> operator +() const;

        /**
         * Makes this parse node optional.
         * @return an optional parse node.
         */
        optional_parse_node<Impl> operator -() const;

        /**
         * Uses this parse node as a logical AND predicate.
         * @return a logical AND parse node.
         */
        logical_and_parse_node<Impl> operator &() const;

        /**
         * Uses this parse node as a logical NOT predicate.
         * @return a logical NOT parse node.
         */
        logical_not_parse_node<Impl> operator !() const;

        /**
         * Creates a parse_node_ptr instance out of this parse node.
         * @return a generic parse_node_ptr instance out of this parse node.
         */
        auto operator ~() const;
    };


    /**
     * Function that returns the implementation of a parse node.
     * @return the implementation of a parse node.
     */
    template <class Impl>
    const Impl& make_parse_node(const parse_node<Impl>& parse_node) {
        return *parse_node.get_impl();
    }


} //namespace parserlib


#endif //PARSERLIB_PARSE_NODE_HPP
