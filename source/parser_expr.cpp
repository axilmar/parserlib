#include "parser_expr.hpp"
#include "char_parser.hpp"
#include "string_parser.hpp"
#include "rule_parser.hpp"
#include "loop_parser.hpp"
#include "optional_parser.hpp"
#include "and_parser.hpp"
#include "not_parser.hpp"
#include "sequence_parser.hpp"
#include "choice_parser.hpp"
#include "term_parser.hpp"
#include "set_parser.hpp"
#include "range_parser.hpp"
#include "newline_parser.hpp"
#include "eof_parser.hpp"


namespace parserlib {


/** constructs a parser expression out of a pointer to a parser object.
    @param p pointer to a parser object.
 */
parser_expr::parser_expr(const parser_object_ptr &p) :
    m_parser(p)
{
}


/** constructs a parser expression out of a single character.
    @param c character.
 */
parser_expr::parser_expr(input_char c) :
    m_parser(new char_parser(c))
{
}


/** constructs a parser expression out of a null-terminated string.
    @param s string.
 */
parser_expr::parser_expr(const char *s) :
    m_parser(new string_parser(s))
{
}


/** constructs a parser expression out of a null-terminated string.
    @param s string.
 */
parser_expr::parser_expr(const wchar_t *s) :
    m_parser(new string_parser(s))
{
}


/** constructs a parser expression out of a rule.
    @param r rule.
 */
parser_expr::parser_expr(rule &r) :
    m_parser(new rule_parser(r))
{
}


/** creates a loop that parses the expression zero or more times.
    @return the appropriate parser expression.
 */
parser_expr parser_expr::operator *() const {
    return new loop_parser(m_parser);
}


/** creates a loop that parses the expression one or more times.
    @return the appropriate parser expression.
 */
parser_expr parser_expr::operator +() const {
    return new sequence_parser(m_parser, new loop_parser(m_parser));
}


/** makes the expression optional.
    @return the appropriate parser expression.
 */
parser_expr parser_expr::operator -() const {
    return new optional_parser(m_parser);
}


/** the logical AND operator.
    @return the appropriate parser expression.
 */
parser_expr parser_expr::operator &() const {
    return new and_parser(m_parser);
}


/** the logical NOT operator.
    @return the appropriate parser expression.
 */
parser_expr parser_expr::operator !() const {
    return new not_parser(m_parser);
}


/** returns the pointer to the internal parser.
    @return the pointer to the internal parser.
 */
const parser_object_ptr &parser_expr::parser() const {
    return m_parser;
}


/** creates a sequence of expressions.
    @param left left expression.
    @param right right expression.
    @return the appropriate parser expression.
 */
parser_expr operator >> (const parser_expr &left, const parser_expr &right) {
    return create_sequence(left.parser(), right.parser());
}


/** creates a choice of expressions.
    @param left left expression.
    @param right right expression.
    @return the appropriate parser expression.
 */
parser_expr operator | (const parser_expr &left, const parser_expr &right) {
    return create_choice(left.parser(), right.parser());
}


/** creates a parser expression that is a terminal.
    Whitespace is not parsed within terminals.
    @param e expression to convert to terminal.
    @return the appropriate parser expression.
 */
parser_expr term(const parser_expr &e) {
    return new term_parser(e.parser());
}


/** creates a parser expression that is a set.
    @param s null-terminated string with characters of the set.
    @return the appropriate parser expression.
 */
parser_expr set(const wchar_t *s) {
    return new set_parser(s);
}


/** creates a parser expression that is a range.
    @param min min character.
    @param max max character.
    @return the appropriate parser expression.
 */
parser_expr range(input_char min, input_char max) {
    return new range_parser(min, max);
}


/** creates a parser expression that handles newlines.
    @param e expression to wrap into a newline parser expression.
    @return the appropriate parser expression.
 */
parser_expr nl(const parser_expr &e) {
    return new newline_parser(e.parser());
}


/** creates a parser expression that tests if the end of file has been reached.
    @return the appropriate parser expression.    
 */
parser_expr eof() {
    return new eof_parser();
}


} //namespace parserlib
