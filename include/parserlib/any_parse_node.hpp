#ifndef PARSERLIB_ANY_PARSE_NODE_HPP
#define PARSERLIB_ANY_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that parses any symbol.
     */ 
    class any_parse_node : public parse_node<any_parse_node> {
    public:
        /**
         * Parses the next available symbol.
         * @param pc the parse context to use.
         * @return true if the end has not been reached, false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            if (pc.is_valid_parse_position()) {
                pc.increment_parse_position();
                return true;
            }
            return false;
        }
    };


    /**
     * An any parse node instance.
     */
    inline const any_parse_node any;


} //namespace parserlib


#endif //PARSERLIB_ANY_PARSE_NODE_HPP
