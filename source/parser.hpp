#ifndef PARSER_HPP
#define PARSER_HPP


//gcc chokes without rule::rule(const rule &),
//msvc complains when rule::rule(const rule &) is defined.
#ifdef _MSC_VER
#pragma warning (disable: 4521)
#endif


#include <vector>
#include <string>
#include <list>


namespace parserlib {


class _private;
class _expr;
class rule;


///type of the parser's input.
typedef std::vector<int> input;


///position into the input.
class pos {
public:
    ///interator into the input.
    input::iterator m_it;

    ///line.
    int m_line;

    ///column.
    int m_col;

    ///null constructor.
    pos() {}

    /** constructor from input.
        @param i input.
     */
    pos(input &i);
};


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


/** type of procedure to invoke when a rule is successfully parsed.
    @param b begin position of input.
    @param e end position of input.
    @param d pointer to user data.
 */
typedef void (*parse_proc)(const pos &b, const pos &e, void *d);


///error.
class error {
public:
    ///begin position.
    pos m_begin;

    ///end position.
    pos m_end;

    ///message.
    std::wstring m_msg;
    
    /** constructor.
        @param b begin position.
        @param e end position.
        @param m message.
     */
    error(const pos &b, const pos &e, const char *m);

    /** constructor.
        @param b begin position.
        @param e end position.
        @param m message.
     */
    error(const pos &b, const pos &e, const wchar_t *m);

    /** compare on begin position. 
        @param e the other error to compare this with.
        @return true if this comes before the previous error, false otherwise.
     */
    bool operator < (const error &e) const;
};


///type of error list.
typedef std::list<error> error_list;


/** represents a rule.
 */
class rule {
public:
    /** constructor from expression.
        @param e expression.
     */
    rule(const expr &e);

    /** constructor from rule.
        @param r rule.
     */
    rule(rule &r);

    /** invalid constructor from rule (required by gcc).
        @param r rule.
        @exception std::logic_error always thrown.
     */
    rule(const rule &r);

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

    /** sets the parse procedure.
        @param p procedure.
     */
    void set_parse_proc(parse_proc p);

    /** get the this ptr (since operator & is overloaded).
        @return pointer to this.
     */
    rule *this_ptr() { return this; }

private:
    //internal expression
    _expr *m_expr;

    //associated parse procedure.
    parse_proc m_parse_proc;

    //assignment not allowed
    rule &operator = (rule &);

    friend class _private;
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


/** creates an expression which tests for the end of input.
    @return an expression that handles the end of input.
 */
expr eof();


/** parses the given input.
    The parse procedures of each rule parsed are executed
    before this function returns, if parsing succeeds.
    @param i input.
    @param g root rule of grammar.
    @param ws whitespace rule.
    @param el list of errors.
    @param d user data, passed to the parse procedures.
    @return true on parsing success, false on failure.
 */
bool parse(input &i, rule &g, rule &ws, error_list &el, void *d);


} //namespace parserlib


#endif //PARSER_HPP
