#ifndef PARSERLIB_PARSE_NODE_WRAPPER_HPP
#define PARSERLIB_PARSE_NODE_WRAPPER_HPP


#include <memory>


namespace parserlib {


    /**
     * Base class for parse node wrappers.
     * @param ParseContext type of parse context to use for parsing.
     */
    template <class ParseContext>
    class parse_node_wrapper {
    public:
        /**
         * Virtual destructor, due to polymorphism.
         */
        virtual ~parse_node_wrapper() {
        }

        /**
         * Interface for parsing.
         * @param pc the parse context to use.
         * @return true on success, false otherwise.
         */
        virtual bool parse(ParseContext& pc) const = 0;

        /**
         * Interface for retrieving the text representation of the wrapped parse node.
         * @return the text representation of the wrapped parse node.
         */
        virtual std::string text() const = 0;

        #ifndef NDEBUG
        virtual void init() const = 0;
        #endif
    };


    /**
     * A parse node wrapper implementation.
     * @param ParseContext type of parse context to use for parsing.
     * @param ParseNode type of parse node to use for parsing.
     */
    template <class ParseContext, class ParseNode>
    class parse_node_wrapper_implementation : public parse_node_wrapper<ParseContext> {
    public:
        /**
         * The constructor.
         * @param parse_node the parse node to use for parsing.
         */
        parse_node_wrapper_implementation(const ParseNode& parse_node) 
            : m_parse_node(parse_node)
        {
        }

        /**
         * Parse function implementation.
         * It invokes the internal parse node.
         * @param pc the parse context to use.
         * @return true on success, false otherwise.
         */
        bool parse(ParseContext& pc) const override {
            return pc.parse(m_parse_node);
        }

        /**
         * Implementation of interface for retrieving the text representation of the wrapped parse node.
         * @return the text representation of the wrapped parse node.
         */
        std::string text() const override {
            return m_parse_node.text();
        }

        #ifndef NDEBUG
        void init() const override {
            m_parse_node.init();
        }
        #endif

    private:
        const ParseNode m_parse_node;
    };


    /**
     * Helper function for creating a parse node wrapper.
     * @param ParseContext the parse context to use.
     * @param parse_node the parse node to use for parsing.
     * @return a parse node wrapper implementation object.
     */
    template <class ParseContext, class ParseNode>
    parse_node_wrapper_implementation<ParseContext, ParseNode> make_parse_node_wrapper(ParseNode&& parse_node) {
        return make_parse_node(parse_node);
    }


    /**
     * Helper function for creating a shared parse node wrapper.
     * @param ParseContext the parse context to use.
     * @param parse_node the parse node to use for parsing.
     * @return a parse node wrapper implementation object shared ptr.
     */
    template <class ParseContext, class ParseNode>
    std::shared_ptr<parse_node_wrapper<ParseContext>> make_shared_parse_node_wrapper(ParseNode&& parse_node) {
        using parse_node_type = decltype(make_parse_node(parse_node));
        using wrapper_type = parse_node_wrapper_implementation<ParseContext, parse_node_type>;
        return std::make_shared<wrapper_type>(make_parse_node(parse_node));
    }


    /**
     * Helper function for creating a unique parse node wrapper.
     * @param ParseContext the parse context to use.
     * @param parse_node the parse node to use for parsing.
     * @return a parse node wrapper implementation object shared ptr.
     */
    template <class ParseContext, class ParseNode>
    std::unique_ptr<parse_node_wrapper<ParseContext>> make_unique_parse_node_wrapper(ParseNode&& parse_node) {
        using parse_node_type = decltype(make_parse_node(parse_node));
        using wrapper_type = parse_node_wrapper_implementation<ParseContext, parse_node_type>;
        return std::make_unique<wrapper_type>(make_parse_node(parse_node));
    }


} //namespace parserlib


#endif //PARSERLIB_PARSE_NODE_WRAPPER_HPP
