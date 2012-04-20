#ifndef PARSERLIB_PARSER_OBJECT_HPP
#define PARSERLIB_PARSER_OBJECT_HPP


#include "shared_object.hpp"
#include "shared_ptr.hpp"
#include "parse_node_ptr.hpp"


namespace parserlib {


class parse_context;
class input_position;


/** base class for parser objects.
    It defines the interface for parsing an input.
 */
class parser_object : public shared_object {
public:
    /** since the object will be subclassed and contains virtual methods,
        the destructor needs to be virtual.
     */
    virtual ~parser_object() {}
    
    /** parsing interface.
        @param context the current parse context.
        @param parent parent parse tree node; children nodes add themselves to this node.
        @param pos parse position; parsing continues from this position, if successful. 
        @param parse_ws if true, whitespace is parsed between terminals.
        @return true if parsing succeeded, false otherwise.
        @exception left_recursion_success thrown if left recursion is successfully parsed.
     */
    virtual bool parse(parse_context &context, const parse_node_ptr &parent, input_position &pos, bool parse_ws) const = 0;
}; 


///pointer to parser object.
typedef shared_ptr<parser_object> parser_object_ptr;


} //namespace parserlib


#endif //PARSERLIB_PARSER_OBJECT_HPP
