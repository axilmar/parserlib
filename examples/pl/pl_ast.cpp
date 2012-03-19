#include <sstream>
#include "pl_ast.hpp"
using namespace std;
using namespace parserlib;


//write spaces for identation
static void _write_ident(FILE *file, int ident) {
    for(; ident > 0; --ident) {
        fprintf(file, " ");
    }
}


/**** TERMINALS ****/


//constructs the identifier.
void ast_identifier::construct(ast_stack &st) {
    for(input::const_iterator it = m_begin.m_it; it != m_end.m_it; ++it) {
        m_value += (char)*it;
    }
}


//emit code
void ast_identifier::emit_code(FILE *file, int ident) {
    fprintf(file, "%s", m_value.c_str());
}


//constructs the value.
void ast_float_literal::construct(ast_stack &st) {
    stringstream stream;
    for(input::const_iterator it = m_begin.m_it; it != m_end.m_it; ++it) {
        stream << (char)*it;
    }
    stream >> m_value;
}


//emit code
void ast_float_literal::emit_code(FILE *file, int ident) {
    fprintf(file, "%f", (double)m_value);
}


//constructs the value.
void ast_int_literal::construct(ast_stack &st) {
    stringstream stream;
    for(input::const_iterator it = m_begin.m_it; it != m_end.m_it; ++it) {
        stream << (char)*it;
    }
    stream >> m_value;
}


//emit code
void ast_int_literal::emit_code(FILE *file, int ident) {
    fprintf(file, "%i", m_value);
}


//constructs the value.
void ast_string_literal::construct(ast_stack &st) {
    stringstream stream;
    for(input::const_iterator it = m_begin.m_it; it != m_end.m_it; ++it) {
        stream << (char)*it;
    }
    m_value = stream.str();
    m_value.erase(m_value.begin());
    m_value.erase(m_value.rbegin().base() - 1);
}


//emit code
void ast_string_literal::emit_code(FILE *file, int ident) {
    fprintf(file, "\"%s\"", m_value.c_str());
}


//constructs the value.
void ast_char_literal::construct(ast_stack &st) {
    stringstream stream;
    for(input::const_iterator it = m_begin.m_it; it != m_end.m_it; ++it) {
        stream << (char)*it;
    }
    string str = stream.str();
    m_value = str[1];
}


//emit code
void ast_char_literal::emit_code(FILE *file, int ident) {
    fprintf(file, "\'%c\'", m_value);
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


//emit code
void ast_bool_literal::emit_code(FILE *file, int ident) {
    fprintf(file, "%i", m_value ? 1 : 0);
}


/**** EXPRESSIONS ****/


//emit code
void ast_func_call::emit_code(FILE *file, int ident) {
    fprintf(file, "%s(", m_name->m_value.c_str());
    int c = 0;
    for(ast_list<ast_expr>::container::const_iterator it = m_args.objects().begin();
        it != m_args.objects().end();
        ++it)
    {
        if (c) fprintf(file, ", ");
        (*it)->emit_code(file, ident);
        c = 1;
    }
    fprintf(file, ")");
}


//emit code
void ast_member_access::emit_code(FILE *file, int ident) {
    int c = 0;
    for(ast_list<ast_identifier>::container::const_iterator it = m_path.objects().begin();
        it != m_path.objects().end();
        ++it)
    {
        if (c) fprintf(file, ".");
        (*it)->emit_code(file, ident);
        c = 1;
    }
}


//emit code
void ast_log_not_expr::emit_code(FILE *file, int ident) {
    fprintf(file, "!");
    m_expr->emit_code(file, ident);
}


//emit code
void ast_positive_expr::emit_code(FILE *file, int ident) {
    fprintf(file, "+");
    m_expr->emit_code(file, ident);
}


//emit code
void ast_negative_expr::emit_code(FILE *file, int ident) {
    fprintf(file, "-");
    m_expr->emit_code(file, ident);
}


//emit code
void ast_mul_expr::emit_code(FILE *file, int ident) {
    m_left->emit_code(file, ident);
    fprintf(file, " * ");
    m_right->emit_code(file, ident);
}


//emit code
void ast_div_expr::emit_code(FILE *file, int ident) {
    m_left->emit_code(file, ident);
    fprintf(file, " / ");
    m_right->emit_code(file, ident);
}


//emit code
void ast_add_expr::emit_code(FILE *file, int ident) {
    m_left->emit_code(file, ident);
    fprintf(file, " + ");
    m_right->emit_code(file, ident);
}


//emit code
void ast_sub_expr::emit_code(FILE *file, int ident) {
    m_left->emit_code(file, ident);
    fprintf(file, " - ");
    m_right->emit_code(file, ident);
}


//emit code
void ast_eq_expr::emit_code(FILE *file, int ident) {
    m_left->emit_code(file, ident);
    fprintf(file, " == ");
    m_right->emit_code(file, ident);
}


//emit code
void ast_diff_expr::emit_code(FILE *file, int ident) {
    m_left->emit_code(file, ident);
    fprintf(file, " != ");
    m_right->emit_code(file, ident);
}


//emit code
void ast_lt_expr::emit_code(FILE *file, int ident) {
    m_left->emit_code(file, ident);
    fprintf(file, " < ");
    m_right->emit_code(file, ident);
}


//emit code
void ast_lte_expr::emit_code(FILE *file, int ident) {
    m_left->emit_code(file, ident);
    fprintf(file, " <= ");
    m_right->emit_code(file, ident);
}


//emit code
void ast_gt_expr::emit_code(FILE *file, int ident) {
    m_left->emit_code(file, ident);
    fprintf(file, " > ");
    m_right->emit_code(file, ident);
}


//emit code
void ast_gte_expr::emit_code(FILE *file, int ident) {
    m_left->emit_code(file, ident);
    fprintf(file, " >= ");
    m_right->emit_code(file, ident);
}


//emit code
void ast_log_or_expr::emit_code(FILE *file, int ident) {
    m_left->emit_code(file, ident);
    fprintf(file, " || ");
    m_right->emit_code(file, ident);
}


//emit code
void ast_log_and_expr::emit_code(FILE *file, int ident) {
    m_left->emit_code(file, ident);
    fprintf(file, " && ");
    m_right->emit_code(file, ident);
}


//emit code
void ast_cond_expr::emit_code(FILE *file, int ident) {
    m_cond->emit_code(file, ident);
    fprintf(file, " ? ");
    m_then->emit_code(file, ident);
    fprintf(file, " : ");
    m_else->emit_code(file, ident);
}


/**** STATEMENTS ****/


//emit code
void ast_var_stm::emit_code(FILE *file, int ident) {
    if (ident) _write_ident(file, ident);
    m_var_def->emit_code(file, ident);
    if (ident) fprintf(file, ";\n");
}


//emit code
void ast_block_stm::emit_code(FILE *file, int ident) {
    _write_ident(file, ident);

    fprintf(file, "{\n");

    for(ast_list<ast_stm>::container::const_iterator it = m_stms.objects().begin();
        it != m_stms.objects().end();
        ++it)
    {
        (*it)->emit_code(file, ident + 4);
    }

    _write_ident(file, ident);
    fprintf(file, "}\n");
}


//emit code
void ast_for_init::emit_code(FILE *file, int ident) {
    m_stm->emit_code(file, ident);
}


//emit code
void ast_for_cond::emit_code(FILE *file, int ident) {
    m_expr->emit_code(file, ident);
}


//emit code
void ast_for_step::emit_code(FILE *file, int ident) {
    m_assignment->emit_code(file, ident);
}


//emit code
void ast_for_stm::emit_code(FILE *file, int ident) {
    _write_ident(file, ident);
    fprintf(file, "for (");
    if (m_init) m_init->emit_code(file, 0);
    fprintf(file, "; ");
    if (m_cond) m_cond->emit_code(file, 0);
    fprintf(file, "; ");
    if (m_step) m_step->emit_code(file, 0);
    fprintf(file, ")\n");
    m_body->emit_code(file, ident);
}


//emit code
void ast_while_stm::emit_code(FILE *file, int ident) {
    _write_ident(file, ident);
    fprintf(file, "while (");
    m_cond->emit_code(file, ident);
    fprintf(file, ")\n");
    m_body->emit_code(file, ident);
}


//emit code
void ast_else_if_stm::emit_code(FILE *file, int ident) {
    _write_ident(file, ident);
    fprintf(file, "else\n");
    m_if->emit_code(file, ident);
}


//emit code
void ast_else_block_stm::emit_code(FILE *file, int ident) {
    _write_ident(file, ident);
    fprintf(file, "else\n");
    m_block->emit_code(file, ident);
}


//emit code
void ast_if_stm::emit_code(FILE *file, int ident) {
    _write_ident(file, ident);
    fprintf(file, "if (");
    m_cond->emit_code(file, ident);
    fprintf(file, ")\n");
    m_then->emit_code(file, ident);
    if (m_else) m_else->emit_code(file, ident);
}


//emit code
void ast_print_stm::emit_code(FILE *file, int ident) {
    _write_ident(file, ident);
    fprintf(file, "cout");

    for(ast_list<ast_expr>::container::const_iterator it = m_exprs.objects().begin();
        it != m_exprs.objects().end();
        ++it)
    {
        fprintf(file, " << ");
        (*it)->emit_code(file, ident);
    }

    fprintf(file, ";\n");
}


//return statement
void ast_return_stm::emit_code(FILE *file, int ident) {
    _write_ident(file, ident);
    fprintf(file, "return");
    if (m_expr) {
        fprintf(file, " ");
        m_expr->emit_code(file, ident);
    }
    fprintf(file, ";\n");
}


//assignment statement
void ast_assignment_stm::emit_code(FILE *file, int ident) {
    _write_ident(file, ident);
    m_left->emit_code(file, ident);
    fprintf(file, " = ");
    m_right->emit_code(file, ident);
    if (ident) fprintf(file, ";\n");
}


//expression statement
void ast_expr_stm::emit_code(FILE *file, int ident) {
    _write_ident(file, ident);
    m_expr->emit_code(file, ident);
    fprintf(file, ";\n");
}


/**** DECLARATIONS ****/


//emit code
void ast_struct_decl::emit_code(FILE *file, int ident) {
    _write_ident(file, ident);
    fprintf(file, "struct %s\n", m_name->m_value.c_str());
    _write_ident(file, ident);
    fprintf(file, "{\n");

    //write member variables
    for(ast_list<ast_var_inst>::container::const_iterator it = m_member_vars.objects().begin();
        it != m_member_vars.objects().end();
        ++it)
    {
        _write_ident(file, ident + 4);
        (*it)->emit_code(file, ident);
        fprintf(file, ";\n");
    }

    _write_ident(file, ident);
    fprintf(file, "};\n\n");

    fprintf(file, "inline ostream &operator << (ostream &stream, const %s &var)\n", m_name->m_value.c_str());
    _write_ident(file, ident);
    fprintf(file, "{\n");
    _write_ident(file, ident);
    if (!m_member_vars.objects().empty()) {
        _write_ident(file, ident + 4);
        fprintf(file, "cout << \"{\"");
        int c = 0;
        for(ast_list<ast_var_inst>::container::const_iterator it = m_member_vars.objects().begin();
            it != m_member_vars.objects().end();
            ++it)
        {
            if (c) {
                fprintf(file, " << \", \" << var.%s", (*it)->m_name->m_value.c_str());
            }
            else {
                fprintf(file, " << var.%s", (*it)->m_name->m_value.c_str());
            }
            c = 1;
        }
        fprintf(file, " << \"}\"");
        fprintf(file, ";\n");
    }
    _write_ident(file, ident + 4);
    fprintf(file, "return stream;\n");
    _write_ident(file, ident);
    fprintf(file, "}\n\n");
}


//emit code
void ast_var_inst::emit_code(FILE *file, int ident) {
    fprintf(file, "%s %s", m_type->name().c_str(), m_name->m_value.c_str());
}


//emit code
void ast_var_init::emit_code(FILE *file, int ident) {
    fprintf(file, " = ");
    m_expr->emit_code(file, ident);
}


//emit code
void ast_var_def::emit_code(FILE *file, int ident) {
    m_var_inst->emit_code(file, ident);
    if (m_init) m_init->emit_code(file, 0);
}


//emit code
void ast_var_decl::emit_code(FILE *file, int ident) {
    _write_ident(file, ident);
    m_var_def->emit_code(file, ident);
    fprintf(file, ";\n\n");
}


//emit code
void ast_func_decl::emit_code(FILE *file, int ident) {
    _write_ident(file, ident);
    fprintf(file, "%s %s(", m_ret_type->name().c_str(), m_name->m_value.c_str());

    //write parameters
    for(ast_list<ast_var_inst>::container::const_iterator it = m_param_vars.objects().begin();
        it != m_param_vars.objects().end();
        ++it)
    {
        if (it != m_param_vars.objects().begin()) fprintf(file, ", ");
        (*it)->emit_code(file, 0);
    }
    fprintf(file, ")\n");

    //write the block statement
    m_body->emit_code(file, ident);

    fprintf(file, "\n");
}


//emit code
void ast_translation_unit::emit_code(FILE *file, int ident) {
    //write default includes
    fprintf(file, "#include <iostream>\n");
    fprintf(file, "#include <string>\n");
    fprintf(file, "using namespace std;\n\n");

    //write declarations
    for(ast_list<ast_declaration>::container::const_iterator it = m_declarations.objects().begin();
        it != m_declarations.objects().end();
        ++it)
    {
        (*it)->emit_code(file, 0);
    }
}
