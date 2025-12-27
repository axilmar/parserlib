#ifndef PARSERLIB_PARSE_NODE_WRAPPER_HPP
#define PARSERLIB_PARSE_NODE_WRAPPER_HPP


namespace parserlib {


    /**
     * Base class for parse node wrapper implementations.
     * @param ParseContext the parse context type to use for parsing.
     */
    template <class ParseContext>
    class parse_node_wrapper {
    public:
        /**
         * The destructor.
         * Virtual due to polymorphism.
         */
        virtual ~parse_node_wrapper() {
        }

        /**
         * Interface for parsing.
         * @param pc the parse context to use.
         * @return true on success, false on failure.
         */
        virtual bool parse(ParseContext& pc) const = 0;
    };


    /**
     * Implementation for parse node wrapping.
     * @param ParseContext the parse context type to use for parsing.
     * @param ParseNode the parse node type to wrap.
     */
    template <class ParseContext, class ParseNode>
    class parse_node_wrapper_impl : public parse_node_wrapper<ParseContext> {
    public:
        /**
         * The constructor.
         * @param parse_node the parse node to parse.
         */
        parse_node_wrapper_impl(const ParseNode& parse_node)
            : m_impl(parse_node)
        {
        }

        /**
         * Invokes the parse function of the wrapped parse node.
         * @param pc the parse context to use.
         * @return true on success, false on failure.
         */ 
        bool parse(ParseContext& pc) const final {
            return m_impl.parse(pc);
        }

    private:
        ParseNode m_impl;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_NODE_WRAPPER_HPP
