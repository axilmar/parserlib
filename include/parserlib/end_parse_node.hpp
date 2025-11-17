#ifndef PARSERLIB_END_PARSE_NODE_HPP
#define PARSERLIB_END_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * Checks for end of input.
     * Useful for checking if the whole input has been parsed.
     */
    class end_parse_node : public parse_node<end_parse_node> {
    public:
        end_parse_node()
            #ifndef NDEBUG
            : m_text("end")
            #endif
        {

        }

        /**
         * Checks for end of input.
         * @param pc the current parse context.
         * @return true if the parse context's iterator reached the end iterator, false otherwise.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return pc.parse_ended();
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
     * Creates an end parse node.
     * @return an end parse node.
     */
    inline end_parse_node end() {
        return end_parse_node();
    }


} //namespace parserlib


#endif //PARSERLIB_END_PARSE_NODE_HPP
