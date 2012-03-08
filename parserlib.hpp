#ifndef PARSERLIB_HPP
#define PARSERLIB_HPP


namespace parserlib {


class _expr;
class rule;


/** a grammar expression.
 */
class expr {
public:
    /** character terminal constructor.
        @param c character. 
     */
    expr(int c);
    
    /** null-terminated string terminal constructor.
        @param s null-terminated string.
     */
    expr(const char *s);
    
    /** null-terminated wide string terminal constructor.
        @param s null-terminated string.
     */
    expr(const wchar_t *s);
    
    /** rule reference constructor.
        @param r rule.
     */
    expr(rule &r);
    
    /** creates a zero-or-more loop out of this expression.
        @return a zero-or-more loop expression.
     */
    expr operator *() const;     
    
    /** creates a one-or-more loop out of this expression.
        @return a one-or-more loop expression.
     */
    expr operator +() const;     
    
    /** creates an optional out of this expression.
        @return an optional expression.
     */
    expr operator -() const;     
    
    /** creates an AND-expression.
        @return an AND-expression.
     */
    expr operator &() const;     
    
    /** creates a NOT-expression.
        @return a NOT-expression.
     */
    expr operator !() const;     
    
private:
    //internal expression
    _expr *m_expr;
    
    //internal constructor from internal expression
    expr(_expr *e) : m_expr(e) {}

    //assignment not allowed
    expr &operator = (expr &);    
    
    friend class _private;
}; 


/** represents a rule.
 */
class rule {
public:
    /** constructor from expression.
        @param e expression.
     */
    rule(const expr &e);

    /** deletes the internal object that represents the expression.
     */
    ~rule();
    
    /** creates a zero-or-more loop out of this rule.
        @return a zero-or-more loop rule.
     */
    expr operator *();
    
    /** creates a one-or-more loop out of this rule.
        @return a one-or-more loop rule.
     */
    expr operator +();
    
    /** creates an optional out of this rule.
        @return an optional rule.
     */
    expr operator -(); 
    
    /** creates an AND-expression out of this rule.
        @return an AND-expression out of this rule.
     */
    expr operator &();
    
    /** creates a NOT-expression out of this rule.
        @return a NOT-expression out of this rule.
     */
    expr operator !();
    
private:    
    //internal expression
    _expr *m_expr;

    //assignment not allowed
    rule &operator = (rule &);    
}; 


/** creates a sequence of expressions.
    @param left left operand.
    @param right right operand.
    @return an expression which parses a sequence.
 */
expr operator >> (const expr &left, const expr &right); 


/** creates a choice of expressions.
    @param left left operand.
    @param right right operand.
    @return an expression which parses a choice.
 */
expr operator | (const expr &left, const expr &right); 


/** converts a parser expression into a terminal.
    @param e expression.
    @return an expression which parses a terminal.
 */
expr term(const expr &e);


/** creates a set expression from a null-terminated string.
    @param s null-terminated string with characters of the set.
    @return an expression which parses a single character out of a set.
 */
expr set(const char *s); 


/** creates a set expression from a null-terminated wide string.
    @param s null-terminated string with characters of the set.
    @return an expression which parses a single character out of a set.
 */
expr set(const wchar_t *s); 


/** creates a range expression.
    @param min min character.
    @param max max character.
    @return an expression which parses a single character out of range.
 */
expr range(int min, int max); 


/** creates an expression which increments the line counter
    and resets the column counter when the given expression
    is parsed successfully; used for newline characters.
    @param e expression to wrap into a newline parser.
    @return an expression that handles newlines.
 */
expr nl(const expr &e); 


} //namespace parserlib


#endif //PARSERLIB_HPP
