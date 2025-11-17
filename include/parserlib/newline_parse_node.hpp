#ifndef PARSERLIB_NEWLINE_PARSE_NODE_HPP
#define PARSERLIB_NEWLINE_PARSE_NODE_HPP


#include "symbol_parse_node.hpp"
#include "string_parse_node.hpp"


namespace parserlib {


    /**
     * A parse node which increments the text position line
     * when another parse node parses successfully.
     * @param ParseNode type of the parse node to use for parsing.
     */
    template <class ParseNode>
    class newline_parse_node : public parse_node<newline_parse_node<ParseNode>> {
    public:
        /**
         * The constructor.
         * @param parse_node the parse node to use for parsing.
         */
        newline_parse_node(const ParseNode& parse_node)
            : m_parse_node(parse_node)
            #ifndef NDEBUG
            , m_text("newline(" + m_parse_node.text() + ")")
            #endif
        {
        }

        /**
         * Using the supplied parse node, it parses the source
         * and then increments the text position line, 
         * if parsing is successful.
         * @param pc the current parse context.
         * @return true on parsing success, false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            if (m_parse_node.parse(pc)) {
                pc.increment_parse_position_line();
                return true;
            }
            return false;
        }

        #ifndef NDEBUG
        const std::string& text() const {
            return m_text;
        }
        #endif

    private:
        const ParseNode m_parse_node;
        #ifndef NDEBUG
        const std::string m_text;
        #endif
    };


    /**
     * Creates a newline parse node out of a symbol.
     * The symbol might also be another parser, or a single character, or a string.
     * @param symbol the symbol to turn into a newline parse node.
     * @return a newline parse node for the given symbol.
     */
    template <class Symbol>
    auto newline(Symbol&& symbol) {
        return newline_parse_node(make_parse_node(std::forward<Symbol>(symbol)));
    }



} //namespace parserlib


#endif //PARSERLIB_NEWLINE_PARSE_NODE_HPP
