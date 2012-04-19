#ifndef PARSERLIB_PARSER_STATE_HPP
#define PARSERLIB_PARSER_STATE_HPP


#include "parse_node.hpp"


namespace parserlib {


/** class that represents the state of the parser that must be locally saved
    in order to backtrack.
 */
class parser_state {
public:
    /** constructor.
        @param parent parent node.
        @param pos current position.
     */
    parser_state(parse_node &parent, input_position &pos);
    
    /** restores the parser state.
        @param parent parent node.
        @param pos current position.
     */
    void restore(parse_node &parent, input_position &pos);

private:
    //number of subnodes in parent
    size_t m_subnodes_count;
    
    //position
    input_position m_position;
}; 


} //namespace parserlib


#endif //PARSERLIB_PARSER_STATE_HPP
