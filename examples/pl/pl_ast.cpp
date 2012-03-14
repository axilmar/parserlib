#include <sstream>
#include "pl_ast.hpp"
using namespace std;
using namespace parserlib;


/**** TERMINALS ****/


//constructs the identifier.
void ast_identifier::construct(ast_stack &st) {
    for(input::const_iterator it = m_begin.m_it; it != m_end.m_it; ++it) {
        m_value += (char)*it;
    }
}


//constructs the value.
void ast_float_literal::construct(ast_stack &st) {
    stringstream stream;
    for(input::const_iterator it = m_begin.m_it; it != m_end.m_it; ++it) {
        stream << (char)*it;
    }
    stream >> m_value;
}


//constructs the value.
void ast_int_literal::construct(ast_stack &st) {
    stringstream stream;
    for(input::const_iterator it = m_begin.m_it; it != m_end.m_it; ++it) {
        stream << (char)*it;
    }
    stream >> m_value;
}


//constructs the value.
void ast_string_literal::construct(ast_stack &st) {
    stringstream stream;
    for(input::const_iterator it = m_begin.m_it; it != m_end.m_it; ++it) {
        stream << (char)*it;
    }
    stream >> m_value;
}


//constructs the value.
void ast_char_literal::construct(ast_stack &st) {
    stringstream stream;
    for(input::const_iterator it = m_begin.m_it; it != m_end.m_it; ++it) {
        stream << (char)*it;
    }
    stream >> m_value;
}


//constructs the value.
void ast_bool_literal::construct(ast_stack &st) {
    stringstream stream;
    for(input::const_iterator it = m_begin.m_it; it != m_end.m_it; ++it) {
        stream << (char)*it;
    }
    string v;
    stream >> v;
    m_value = v == "true";
}


