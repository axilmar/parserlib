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


/**** STATEMENTS ****/


//typecheck.
void ast_var_stm::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    m_var_def->type_check(unit, errors);
}


//typecheck.
void ast_block_stm::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    for(ast_list<ast_stm>::container::const_iterator it = m_stms.objects().begin();
        it != m_stms.objects().end();
        ++it)
    {
        ast_stm *stm = *it;
        stm->type_check(unit, errors);
    }
}


//typecheck.
void ast_for_stm::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    m_init_stm->type_check(unit, errors);
    //TODO typecheck the conditional expression
    m_step->type_check(unit, errors);
    m_body->type_check(unit, errors);
}


//typecheck.
void ast_while_stm::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //TODO typecheck the conditional expression
    m_body->type_check(unit, errors);    
}


//typecheck.
void ast_else_if_stm::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    m_if->type_check(unit, errors);
}


//typecheck.
void ast_else_block_stm::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    m_block->type_check(unit, errors);
}


//typecheck.
void ast_if_stm::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //TODO typecheck the conditional expression
    m_then->type_check(unit, errors);    
    if (m_else) m_else->type_check(unit, errors);
}


//typecheck
void ast_print_stm::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //TODO typecheck all expressions
}


//typecheck.
void ast_return_stm::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //TODO typecheck the return expression
    //TODO also typecheck the return expression with the function return type
}


//typecheck.
void ast_assignment_stm::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //TODO typecheck left and right expressions
}


//typecheck.
void ast_expr_stm::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //TODO typecheck expression
}


/**** DECLARATIONS ****/


//typecheck the code.
void ast_struct_decl::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //typecheck member variables
    for(ast_list<ast_var_inst>::container::const_iterator it = m_member_vars.objects().begin();
        it != m_member_vars.objects().end();
        ++it)
    {
        ast_var_inst *var_inst = *it;
        var_inst->type_check(unit, errors);
        
        //check for recursive struct
        if (var_inst->m_type->name() == m_name->m_value) {
            errors.push_back(error(var_inst->m_type->m_begin, var_inst->m_type->m_end, "recursive struct: " + var_inst->m_type->name()));
        }
    }
}


//type check
void ast_var_inst::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    unit->check_type(m_type, errors);
}

        
//type check
void ast_var_init::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //TODO typecheck the expression
}


//type check
void ast_var_def::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    m_var_inst->type_check(unit, errors);    
    if (m_init) {
        m_init->type_check(unit, errors);
        //TODO typecheck the variable type against the init expression
    }
}


//typecheck the code.
void ast_var_decl::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    m_var_def->type_check(unit, errors);
}


//typecheck the code.
void ast_func_decl::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //typecheck the parameters
    for(ast_list<ast_var_inst>::container::const_iterator it = m_param_vars.objects().begin();
        it != m_param_vars.objects().end();
        ++it)
    {
        ast_var_inst *inst = *it;
        inst->type_check(unit, errors);
    }   
    
    //typecheck the return type
    unit->check_type(m_ret_type, errors);
    
    //typecheck the body
    m_body->type_check(unit, errors);
}


//type check the code
void ast_translation_unit::type_check(parserlib::error_list &errors) {    
    //typecheck all declarations
    for(ast_list<ast_declaration>::container::const_iterator it = m_declarations.objects().begin();
        it != m_declarations.objects().end();
        ++it)
    {
        ast_declaration *decl = *it;
        decl->type_check(this, errors);
    }        
}


//find type
std::string ast_translation_unit::find_type(const std::string &name) const {
    //check for primitive
    if (name == "int") return name;
    if (name == "float") return name;
    if (name == "string") return name;
    if (name == "char") return name;
    if (name == "bool") return name;
    
    //find the appropriate type
    for(ast_list<ast_declaration>::container::const_iterator it = m_declarations.objects().begin();
        it != m_declarations.objects().end();
        ++it)
    {
        ast_declaration *decl = *it;
        if (decl->is_type() && decl->name() == name) {
            return name;
        }
    }        
    
    //not found
    return string();
}


//check if the given type exists; if not, add an error to the error list.
void ast_translation_unit::check_type(ast_type *type, parserlib::error_list &errors) const {
    if (find_type(type->name()).empty()) {
        errors.push_back(error(type->m_begin, type->m_end, "unknown type: " + type->name()));
    }
}

