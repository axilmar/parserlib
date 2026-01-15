#ifndef PARSERLIB_END_PARSE_NODE_HPP
#define PARSERLIB_END_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that checks if the end of input has been reached.
     */ 
    class end_parse_node : public parse_node<end_parse_node> {
    public:
        /**
         * Returns true if the end has been reached.
         * @param pc the parse context to use.
         * @return true if the end has been reached, false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return pc.is_end_parse_position();
        }
    };


    /**
     * An end parse node instance.
     */
    inline const end_parse_node end;


} //namespace parserlib


#endif //PARSERLIB_END_PARSE_NODE_HPP
