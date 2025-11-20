#ifndef PARSERLIB_NAMED_PARSE_NODE_HPP
#define PARSERLIB_NAMED_PARSE_NODE_HPP


namespace parserlib {


    #ifndef NDEBUG
    /**
     * Parse node only used in debugging to attach a name to an expression.
     * Available only in debug mode.
     * @param ParseNode the child parse node type.
     */
    template <class ParseNode>
    class named_parse_node : public parse_node<named_parse_node<ParseNode>> {
    public:
        /**
         * Constructor.
         * @param name name of the expression.
         * @param parse_node the parse node to use for parsing.
         */
        named_parse_node(const std::string& name, const ParseNode& parse_node)
            : m_name(name)
            , m_parse_node(parse_node)
        {
        }

        /**
         * Invokes the child parse node.
         * @param pc the current parse context.
         * @return true on success, false on failure.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return pc.parse(m_parse_node);
        }

        /**
         * Returns the name.
         * @return the name.
         */
        std::string text() const override {
            return m_name;
        }

        /**
         * Returns the parse node that is named.
         * @return the parse node that is named.
         */
        const ParseNode& parse_node() const {
            return m_parse_node;
        }

        /**
         * Returns the name.
         * @return the name.
         */
        const std::string& name() const {
            return m_name;
        }

        /**
         * Sets the name of the parse node.
         * @param name the name of the parse node.
         */
        void set_name(const std::string& name) {
            m_name = name;
        }

        #ifndef NDEBUG
        void init_tree() const override {
            m_parse_node.init();
        }
        #endif

    private:
        const std::string m_name;
        const ParseNode m_parse_node;
    };
    #endif


    /**
     * In debug mode, it creates a named parse node.
     * In release mode, it simply returns the parse node for the given value.
     * @param name the parse node name.
     * @param parse_node the parse node (or value to create a parse node for) to attach a name to.
     * @return in debug mode, a named parse node; it release mode, a parse node for the given value.
     */
    template <class ParseNode>
    auto operator >>= (const char* name, ParseNode&& parse_node) {
        #ifndef NDEBUG
        return named_parse_node(name, make_parse_node(parse_node));
        #else
        return make_parse_node(parse_node);
        #endif
    }


} //namespace parserlib


#endif //PARSERLIB_NAMED_PARSE_NODE_HPP
