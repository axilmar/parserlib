#include <sstream>
#include "pl_ast.hpp"
using namespace std;
using namespace parserlib;


//builtin type indexes
static const char *_builtin_types[] = {
    "bool",
    "char",
    "int",
    "float",
    "string",
    0
};


//get index of builtin types
static int _builtin_type_index(const std::string &type) {
    for(int i = 0; _builtin_types[i]; ++i) {
        if (type == _builtin_types[i]) return i;
    }
    return -1;
}


//check if two types are convertible
static bool _is_convertible_type(const std::string &dst, const std::string &src) {
    //get the index of types
    int dst_index = _builtin_type_index(src);
    int src_index = _builtin_type_index(dst);
    
    //if one of them is -1, then the types must be equal in name
    if (dst_index < 0 || src_index < 0) {
        return dst == src;
    }
    
    //otherwise, the types are convertible only if the destination index is equal 
    //or greater to the source index
    return dst_index >= src_index;
}


//convert input sequence to string
static string _string(const pos &b, const pos &e) {
    stringstream stream;
    for(input::iterator it = b.m_it; it != e.m_it; ++it) {
        stream << (char)*it;
    }
    return stream.str();
}


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
    m_error = stream.fail() || stream.bad();
}


//type check the expression
void ast_float_literal::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    if (m_error) {
        errors.push_back(error(m_begin, m_end, "invalid float: " + _string(m_begin, m_end)));
    }
}


//constructs the value.
void ast_int_literal::construct(ast_stack &st) {
    stringstream stream;
    for(input::const_iterator it = m_begin.m_it; it != m_end.m_it; ++it) {
        stream << (char)*it;
    }
    stream >> m_value;
    m_error = stream.fail() || stream.bad();
}


//type check the expression
void ast_int_literal::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    if (m_error) {
        errors.push_back(error(m_begin, m_end, "invalid int: " + _string(m_begin, m_end)));
    }
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


/**** EXPRESSIONS ****/


//locate the closest variable with the given name
ast_var_inst *ast_expr::find_var(const std::string &name) const {
    //TODO
    return 0;
}


//type check the expression
void ast_func_call::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //find the function
    ast_func_decl *func = dynamic_cast<ast_func_decl *>(unit->find_decl(m_name->m_value));
    
    //if not found
    if (!func) {
        errors.push_back(error(m_begin, m_end, "unknown function: " + m_name->m_value));
        return;
    }
    
    //check the number of parameters against the number of arguments
    if (func->m_param_vars.objects().size() != m_args.objects().size()) {
        errors.push_back(error(m_begin, m_end, "invalid number of arguments"));
    }
    
    //check the common number of parameters for type equality
    ast_list<ast_var_inst>::container::const_iterator itParam = func->m_param_vars.objects().begin(), 
        endParam = func->m_param_vars.objects().end();
    ast_list<ast_expr>::container::const_iterator itArg = m_args.objects().begin(),
        endArg = m_args.objects().end();
    for(; itParam != endParam && itArg != endArg; ++itParam, ++endArg) {
        ast_var_inst *param = *itParam;
        ast_expr *arg = *itArg;
        if (!_is_convertible_type(param->m_type->name(), arg->type_name())) {
            errors.push_back(error(arg->m_begin, arg->m_end, "invalid argument for parameter: " + param->m_name->m_value));            
        }
    }
    
    //set the typename of the function
    m_type_name = func->m_ret_type->name();
}

    
//type check the expression
void ast_member_access::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //get the first identifier
    ast_list<ast_identifier>::container::const_iterator it = m_path.objects().begin();
    ast_identifier *id = *it;
    
    //find the variable this identifier refers to
    ast_var_inst *var = find_var(id->m_value);
    
    //if not found, issue an error
    if (!var) {
        errors.push_back(error(m_begin, m_end, "unknown variable: " + id->m_value));
        return;
    }

    //now that the variable is found, follow the trail of variables and check
    //the members of each one    
    for(++it; it != m_path.objects().end(); ++it) {
        id = *it;
        
        string var_type_name = var->m_type->name();

        //the variable must not be a builtin type
        if (_builtin_type_index(var_type_name) >= 0) {
            errors.push_back(error(m_begin, m_end, "unknown symbol: " + id->m_value));
            break;
        }
        
        //now we know the variable's type is a struct,
        //so find the struct
        ast_struct_decl *strct = dynamic_cast<ast_struct_decl *>(unit->find_decl(var_type_name));
        
        //if not found, then do nothing, because it means there are two or more symbols with the given name
        if (!strct) {
            break;
        }
        
        //now that the struct is found, find the member variable with 'id' as name
        var = strct->find_member_var(id->m_value);
        
        //if not found, create an error
        if (!var) {
            errors.push_back(error(m_begin, m_end, "unknown symbol: " + id->m_value));
            break;
        }
    }
    
    //the typename is the name of the last type found
    if (var) {
        m_type_name = var->m_type->name();
    }
}

    
//type check the expression
void ast_log_not_expr::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //TODO
}

    
//type check the expression
void ast_positive_expr::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //TODO
}

    
//type check the expression
void ast_negative_expr::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //TODO
}

    
//type check the expression
void ast_mul_expr::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //TODO
}


//type check the expression
void ast_div_expr::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //TODO
}


//type check the expression
void ast_add_expr::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //TODO
}


//type check the expression
void ast_sub_expr::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //TODO
}


//type check the expression
void ast_eq_expr::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //TODO
}


//type check the expression
void ast_diff_expr::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //TODO
}


//type check the expression
void ast_lt_expr::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //TODO
}


//type check the expression
void ast_lte_expr::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //TODO
}


//type check the expression
void ast_gt_expr::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //TODO
}


//type check the expression
void ast_gte_expr::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //TODO
}


//type check the expression
void ast_log_or_expr::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //TODO
}


//type check the expression
void ast_log_and_expr::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //TODO
}


//type check the expression
void ast_cond_expr::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    //TODO
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
void ast_for_init::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    m_stm->type_check(unit, errors);
}


//typecheck.
void ast_for_cond::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    m_expr->type_check(unit, errors);
}


//typecheck.
void ast_for_step::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    m_assignment->type_check(unit, errors);
}


//typecheck.
void ast_for_stm::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    if (m_init) m_init->type_check(unit, errors);
    if (m_cond) m_cond->type_check(unit, errors);
    if (m_step) m_step->type_check(unit, errors);
    m_body->type_check(unit, errors);
}


//typecheck.
void ast_while_stm::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    m_cond->type_check(unit, errors);
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
    m_cond->type_check(unit, errors);
    m_then->type_check(unit, errors);    
    if (m_else) m_else->type_check(unit, errors);
}


//typecheck
void ast_print_stm::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    for(ast_list<ast_expr>::container::const_iterator it = m_exprs.objects().begin();
        it != m_exprs.objects().end();
        ++it)
    {
        ast_expr *expr = *it;
        expr->type_check(unit, errors);
    }
}


//typecheck.
void ast_return_stm::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    if (m_expr) {
        m_expr->type_check(unit, errors);
        //TODO typecheck the return expression with the function return type
    }
}


//typecheck.
void ast_assignment_stm::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    m_left->type_check(unit, errors);
    m_right->type_check(unit, errors);
}


//typecheck.
void ast_expr_stm::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    m_expr->type_check(unit, errors);
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


//find member variable
ast_var_inst *ast_struct_decl::find_member_var(const std::string &name) const {
    for(ast_list<ast_var_inst>::container::const_iterator it = m_member_vars.objects().begin();
        it != m_member_vars.objects().end();
        ++it)
    {
        ast_var_inst *inst = *it;
        if (inst->m_name->m_value == name) return inst;    
    }    
    return 0;
}


//type check
void ast_var_inst::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    unit->check_type(m_type, errors);
}

        
//type check
void ast_var_init::type_check(ast_translation_unit *unit, parserlib::error_list &errors) {
    m_expr->type_check(unit, errors);
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

//find a declaration by name
ast_declaration *ast_translation_unit::find_decl(const std::string &name) const {
    for(ast_list<ast_declaration>::container::const_iterator it = m_declarations.objects().begin();
        it != m_declarations.objects().end();
        ++it)
    {
        ast_declaration *decl = *it;
        if (decl->name() == name) return decl;
    }        
    return 0;
}

