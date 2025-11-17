#ifndef PARSERLIB_ANY_PARSE_NODE_HPP
#define PARSERLIB_ANY_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * Accepts any input.
     */
    class any_parse_node : public parse_node<any_parse_node> {
    public:
        any_parse_node()
            #ifndef NDEBUG
            : m_text("any")
            #endif
        {
        }

        /**
         * Accepts any input.
         * @param pc the current parse context; on return, the parse position is advanced by one symbol.
         * @return true if the parse position was valid, false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            if (pc.parse_valid() && pc.terminal_parsing_allowed()) {
                pc.increment_parse_position();
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
        #ifndef NDEBUG
        const std::string m_text;
        #endif
    };


    /**
     * Creates an any parse node.
     * @return an any parse node.
     */
    inline any_parse_node any() {
        return any_parse_node();
    }


} //namespace parserlib


#endif //PARSERLIB_ANY_PARSE_NODE_HPP
