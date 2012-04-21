#include <string>
#include "parse_node.hpp"
#include "rule.hpp"


namespace parserlib {


/** constructor.
    The end position is set from the begin position.
    @param mr the rule that was matched.
    @param begin begin position.
 */
parse_node::parse_node(rule &mr, const input_position &begin) : 
    m_matched_rule(mr.this_()), m_begin_position(begin), m_end_position(begin)
{
}
    

/** returns the matched rule.
    @return the matched rule.
 */
rule &parse_node::matched_rule() const {
    return *m_matched_rule;
}


/** returns the begin position.
    @return the begin position.
 */
const input_position &parse_node::begin_position() const {
    return m_begin_position;
}


/** returns the end position.
    @return the end position.
 */
const input_position &parse_node::end_position() const {
    return m_end_position;
}


/** returns the container of subnodes.
    @return the container of subnodes.
 */
const parse_node_container &parse_node::subnodes() const {
    return m_subnodes;
}


/** adds a subnode.
    @param sn subnode to add.
 */
void parse_node::add_subnode(const parse_node_ptr &sn) {
    m_subnodes.push_back(sn);
    m_end_position = sn->m_end_position;
}


/** sets the container of subnodes to contain N subnode entries.
    @param n number of subnode entries to contain.
 */
void parse_node::resize_subnodes(size_t n) {
    m_subnodes.resize(n);
}


/** sets the end position.
    @param pos the new end position.
 */
void parse_node::set_end_position(const input_position &pos) {
    assert(pos >= m_begin_position);
    m_end_position = pos;
}


/** prints the parse tree to the given stream.
    @param stream stream.
    @param depth depth of tree.
 */
void parse_node::print(std::ostream &stream, size_t depth/* = 0*/) const {
    stream << std::string(depth * 4, ' ') << m_matched_rule->name() << std::endl;
    for(parse_node_container::const_iterator it = m_subnodes.begin();
        it != m_subnodes.end();
        ++it)
    {
        (*it)->print(stream, depth + 1);
    }
}


} //namespace parserlib
