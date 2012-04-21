#ifndef PARSERLIB_PARSE_NODE_HPP
#define PARSERLIB_PARSE_NODE_HPP


#include <ostream>
#include "shared_object.hpp"
#include "input_position.hpp"
#include "parse_node_ptr.hpp"


namespace parserlib {


class rule;


class parse_node;


///container of parse nodes.
typedef std::vector<parse_node_ptr> parse_node_container;


/** a parse node.

    It contains the beginning and ending positions that a rule was parsed successfully within,
    the rule, and the container of parse nodes.
 */
class parse_node : public shared_object {
public:
    /** constructor.
        @param mr the rule that was matched.
        @param begin begin position.
     */
    parse_node(rule &mr, const input_position &begin);
    
    /** returns the matched rule.
        @return the matched rule.
     */
    rule &matched_rule() const;
    
    /** returns the begin position.
        @return the begin position.
     */
    const input_position &begin_position() const;
    
    /** returns the end position.
        @return the end position.
     */
    const input_position &end_position() const;
    
    /** returns the container of subnodes.
        @return the container of subnodes.
     */
    const parse_node_container &subnodes() const;
    
    /** adds a subnode.
        @param sn subnode to add.
     */
    void add_subnode(const parse_node_ptr &sn);
    
    /** sets the container of subnodes to contain N subnode entries.
        @param n number of subnode entries to contain.
     */
    void resize_subnodes(size_t n);
    
    /** sets the end position.
        @param pos the new end position.
     */
    void set_end_position(const input_position &pos);
    
    /** prints the parse tree to the given stream.
        @param stream stream.
        @param depth depth of tree.
     */
    void print(std::ostream &stream, size_t depth = 0) const;

private:
    //matched rule
    rule *m_matched_rule;
    
    //begin position
    input_position m_begin_position;
    
    //end position
    input_position m_end_position;
    
    //container of subnodes.
    parse_node_container m_subnodes;
}; 


} //namespace parserlib


#endif //PARSERLIB_PARSE_NODE_HPP
