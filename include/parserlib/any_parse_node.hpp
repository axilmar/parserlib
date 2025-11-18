#ifndef PARSERLIB_ANY_PARSE_NODE_HPP
#define PARSERLIB_ANY_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * Accepts any input.
     */
    class any_parse_node : public parse_node<any_parse_node> {
    public:
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

        /**
         * Converts the parse node to a textual description.
         * @return a string of this parse node as text.
         */
        std::string text() const override {
            return "any";
        }
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
