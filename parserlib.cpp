#include "parserlib.hpp"


namespace parserlib {


//internal private class that manages access to the public classes' internals.
class _private {
public:
    //get the internal expression object from the expression.
    static _expr *get_expr(const expr &e) {
        return e.m_expr;
    }
    
    //create new expression from given expression
    static expr construct_expr(_expr *expr) {
        return expr;
    }
};


//base class for expressions
class _expr {
public:
    //destructor.
    virtual ~_expr() {
    }
};


//single character expression.
class _char : public _expr {
public:
    //constructor.
    _char(int c) {
        //TODO
    }
};


//string expression.
class _string : public _expr {
public:
    //constructor from ansi string.
    _string(const char *s) {
        //TODO
    }

    //constructor from wide string.
    _string(const wchar_t *s) {
        //TODO
    }
};


//set expression.
class _set : public _expr {
public:
    //constructor from ansi string.
    _set(const char *s) {
        //TODO
    }

    //constructor from wide string.
    _set(const wchar_t *s) {
        //TODO
    }

    //constructor from range.
    _set(int min, int max) {
        //TODO
    }
};


//base class for unary expressions
class _unary : public _expr {
public:
    //constructor.
    _unary(_expr *e) {
        //TODO
    }
};


//terminal
class _term : public _unary {
public:
    //constructor.
    _term(_expr *e) : 
        _unary(e)
    {
    }
};


//loop 0 
class _loop0 : public _unary {
public:
    //constructor.
    _loop0(_expr *e) : 
        _unary(e)
    {
    }
};


//loop 1
class _loop1 : public _unary {
public:
    //constructor.
    _loop1(_expr *e) : 
        _unary(e)
    {
    }
};


//optional
class _optional : public _unary {
public:
    //constructor.
    _optional(_expr *e) : 
        _unary(e)
    {
    }
};


//and
class _and : public _unary {
public:
    //constructor.
    _and(_expr *e) : 
        _unary(e)
    {
    }
};


//not
class _not : public _unary {
public:
    //constructor.
    _not(_expr *e) : 
        _unary(e)
    {
    }
};


//newline
class _nl : public _unary {
public:
    //constructor.
    _nl(_expr *e) : 
        _unary(e)
    {
    }
};


//base class for binary expressions
class _binary : public _expr {
public:
    //constructor.
    _binary(_expr *left, _expr *right) {
        //TODO
    }
};


//sequence
class _seq : public _binary {
public:
    //constructor.
    _seq(_expr *left, _expr *right) : 
        _binary(left, right)
    {
    }
};


//choice
class _choice : public _binary {
public:
    //constructor.
    _choice(_expr *left, _expr *right) : 
        _binary(left, right)
    {
    }
};


//reference to rule
class _ref : public _expr {
public:
    //constructor.
    _ref(rule &r) : 
        m_rule(r)
    {
    }
    
private:
    //reference
    rule &m_rule;
};


/** character terminal constructor.
    @param c character. 
 */
expr::expr(int c) : 
    m_expr(new _char(c))
{
}


/** null-terminated string terminal constructor.
    @param s null-terminated string.
 */
expr::expr(const char *s) :
    m_expr(new _string(s))
{
}


/** null-terminated wide string terminal constructor.
    @param s null-terminated string.
 */
expr::expr(const wchar_t *s) :
    m_expr(new _string(s))
{
}


/** rule reference constructor.
    @param r rule.
 */
expr::expr(rule &r) :
    m_expr(new _ref(r))
{
}


/** creates a zero-or-more loop out of this expression.
    @return a zero-or-more loop expression.
 */
expr expr::operator *() const {
    return new _loop0(m_expr);
}


/** creates a one-or-more loop out of this expression.
    @return a one-or-more loop expression.
 */
expr expr::operator +() const {
    return new _loop1(m_expr);
}


/** creates an optional out of this expression.
    @return an optional expression.
 */
expr expr::operator -() const {
    return new _optional(m_expr);
}


/** creates an AND-expression.
    @return an AND-expression.
 */
expr expr::operator &() const {
    return new _and(m_expr);
}


/** creates a NOT-expression.
    @return a NOT-expression.
 */
expr expr::operator !() const {
    return new _not(m_expr);
}

    
/** constructor from expression.
    @param e expression.
 */
rule::rule(const expr &e) :
    m_expr(_private::get_expr(e))
{
}


/** deletes the internal object that represents the expression.
 */
rule::~rule() {
    delete m_expr;
}


/** creates a zero-or-more loop out of this rule.
    @return a zero-or-more loop rule.
 */
expr rule::operator *() {
    return new _loop0(new _ref(*this));
}


/** creates a one-or-more loop out of this rule.
    @return a one-or-more loop rule.
 */
expr rule::operator +() {
    return new _loop1(new _ref(*this));
}


/** creates an optional out of this rule.
    @return an optional rule.
 */
expr rule::operator -() {
    return new _optional(new _ref(*this));
}


/** creates an AND-expression out of this rule.
    @return an AND-expression out of this rule.
 */
expr rule::operator &() {
    return new _and(new _ref(*this));
}


/** creates a NOT-expression out of this rule.
    @return a NOT-expression out of this rule.
 */
expr rule::operator !() {
    return new _not(new _ref(*this));
}


/** creates a sequence of expressions.
    @param left left operand.
    @param right right operand.
    @return an expression which parses a sequence.
 */
expr operator >> (const expr &left, const expr &right) {
    return new _seq(_private::get_expr(left), _private::get_expr(right));
}


/** creates a choice of expressions.
    @param left left operand.
    @param right right operand.
    @return an expression which parses a choice.
 */
expr operator | (const expr &left, const expr &right) {
    return new _choice(_private::get_expr(left), _private::get_expr(right));
}


/** converts a parser expression into a terminal.
    @param e expression.
    @return an expression which parses a terminal.
 */
expr term(const expr &e) {
    return new _term(_private::get_expr(e));
}


/** creates a set expression from a null-terminated string.
    @param s null-terminated string with characters of the set.
    @return an expression which parses a single character out of a set.
 */
expr set(const char *s) {
    return new _set(s);
}


/** creates a set expression from a null-terminated wide string.
    @param s null-terminated string with characters of the set.
    @return an expression which parses a single character out of a set.
 */
expr set(const wchar_t *s) {
    return new _set(s);
}


/** creates a range expression.
    @param min min character.
    @param max max character.
    @return an expression which parses a single character out of range.
 */
expr range(int min, int max) {
    return new _set(min, max);
}


/** creates an expression which increments the line counter
    and resets the column counter when the given expression
    is parsed successfully; used for newline characters.
    @param e expression to wrap into a newline parser.
    @return an expression that handles newlines.
 */
expr nl(const expr &e) {
    return new _nl(_private::get_expr(e));
}


} //namespace parserlib
