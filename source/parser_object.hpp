#ifndef PARSERLIB_PARSER_OBJECT_HPP
#define PARSERLIB_PARSER_OBJECT_HPP


#include "shared_object.hpp"
#include "parse_context.hpp"
#include "parse_node.hpp"


namespace parserlib {


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
        @return true if parsing succeeded, false otherwise.
        @exception left_recursion_success thrown if left recursion is successfully parsed.
     */
    virtual bool parse(parse_context &context, parse_node &parent, input_position &pos) const = 0;
}; 


} //namespace parserlib


#endif //PARSERLIB_PARSER_OBJECT_HPP
