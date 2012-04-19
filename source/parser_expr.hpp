#ifndef PARSERLIB_PARSER_EXPR_HPP
#define PARSERLIB_PARSER_EXPR_HPP


#include "parser_object.hpp"


namespace parserlib {


class rule;


/** a class that represents a container for a parser expression.
 */
class parser_expr {
public:
    /** constructs a parser expression out of a pointer to a parser object.
        @param p pointer to a parser object.
     */
    parser_expr(const parser_object_ptr &p);
    
    /** constructs a parser expression out of a single character.
        @param c character.
     */
    parser_expr(input_char c);
    
    /** constructs a parser expression out of a null-terminated string.
        @param s string.
     */
    parser_expr(const char *s);
    
    /** constructs a parser expression out of a null-terminated string.
        @param s string.
     */
    parser_expr(const wchar_t *s);
    
    /** constructs a parser expression out of a rule.
        @param r rule.
     */
    parser_expr(rule &r);
    
    /** creates a loop that parses the expression zero or more times.
        @return the appropriate parser expression.
     */
    parser_expr operator *() const;
    
    /** creates a loop that parses the expression one or more times.
        @return the appropriate parser expression.
     */
    parser_expr operator +() const;
    
    /** makes the expression optional.
        @return the appropriate parser expression.
     */
    parser_expr operator -() const;
    
    /** the logical AND operator.
        @return the appropriate parser expression.
     */
    parser_expr operator &() const;
    
    /** the logical NOT operator.
        @return the appropriate parser expression.
     */
    parser_expr operator !() const;
    
    /** returns the pointer to the internal parser.
        @return the pointer to the internal parser.
     */
    const parser_object_ptr &parser() const;
    
private:
    //pointer to parser object.
    parser_object_ptr m_parser;    
}; 


/** creates a sequence of expressions.
    @param left left expression.
    @param right right expression.
    @return the appropriate parser expression.
 */
parser_expr operator >> (const parser_expr &left, const parser_expr &right); 


/** creates a choice of expressions.
    @param left left expression.
    @param right right expression.
    @return the appropriate parser expression.
 */
parser_expr operator | (const parser_expr &left, const parser_expr &right); 


/** creates a parser expression that is a set.
    @param s null-terminated string with characters of the set.
    @return the appropriate parser expression.
 */
parser_expr set(const char *s); 


/** creates a parser expression that is a terminal.
    Whitespace is not parsed within terminals.
    @param e expression to convert to terminal.
    @return the appropriate parser expression.
 */
parser_expr term(const parser_expr &e); 


/** creates a parser expression that is a set.
    @param s null-terminated string with characters of the set.
    @return the appropriate parser expression.
 */
parser_expr set(const wchar_t *s); 


/** creates a parser expression that is a range.
    @param min min character.
    @param max max character.
    @return the appropriate parser expression.
 */
parser_expr range(input_char min, input_char max); 


/** creates a parser expression that handles newlines.
    @param e expression to wrap into a newline parser expression.
    @return the appropriate parser expression.
 */
parser_expr nl(const parser_expr &e);


/** creates a parser expression that tests if the end of file has been reached.
    @return the appropriate parser expression.    
 */
parser_expr eof(); 


} //namespace parserlib


#endif //PARSERLIB_PARSER_EXPR_HPP
