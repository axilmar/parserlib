#include "parser_state.hpp"
#include "parse_node.hpp"


namespace parserlib {


/** constructor.
    @param parent parent node.
    @param pos current position.
 */
parser_state::parser_state(const parse_node_ptr &parent, input_position &pos) :
    m_subnodes_count(parent->subnodes().size()), m_position(pos)
{
}


/** restores the parser state.
    @param parent parent node.
    @param pos current position.
 */
void parser_state::restore(const parse_node_ptr &parent, input_position &pos) {
    parent->resize_subnodes(m_subnodes_count);
    pos = m_position;
}


} //namespace parserlib