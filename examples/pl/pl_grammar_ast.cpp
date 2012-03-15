#include "pl_grammar.hpp"
#include "pl_ast.hpp"
using namespace parserlib;


/******************************************************************************
    GRAMMAR-AST
 ******************************************************************************/


/**** TERMINALS ****/


ast<ast_identifier> identifier_ast(identifier);
ast<ast_float_literal> float_literal_ast(float_literal);
ast<ast_int_literal> int_literal_ast(int_literal);
ast<ast_string_literal> string_literal_ast(string_literal);
ast<ast_char_literal> char_literal_ast(char_literal);
ast<ast_bool_literal> bool_literal_ast(bool_literal);


/**** TYPES ****/


ast<ast_void_type> void_type_ast(void_type);
ast<ast_int_type> int_type_ast(int_type);
ast<ast_float_type> float_type_ast(float_type);
ast<ast_bool_type> bool_type_ast(bool_type);
ast<ast_string_type> string_type_ast(string_type);
ast<ast_char_type> char_type_ast(char_type);
ast<ast_id_type> id_type_ast(id_type);


/**** EXPRESSIONS ****/


ast<ast_func_call> func_call_ast(function_call);
ast<ast_member_access> member_access_ast(member_access);
ast<ast_log_not_expr> log_not_expr_ast(logical_not_expr);
ast<ast_positive_expr> positive_expr_ast(positive_expr);
ast<ast_negative_expr> negative_expr_ast(negative_expr);
ast<ast_mul_expr> mul_expr_ast(mul_op);
ast<ast_div_expr> div_expr_ast(div_op);
ast<ast_add_expr> add_expr_ast(add_op);
ast<ast_sub_expr> sub_expr_ast(sub_op);
ast<ast_eq_expr> eq_op_expr_ast(eq_op);
ast<ast_diff_expr> diff_op_expr_ast(diff_op);
ast<ast_lt_expr> lt_op_expr_ast(lt_op);
ast<ast_lte_expr> lte_op_expr_ast(lte_op);
ast<ast_gt_expr> gt_op_expr_ast(gt_op);
ast<ast_gte_expr> gte_op_expr_ast(gte_op);
ast<ast_log_and_expr> log_and_expr_ast(log_and_op);
ast<ast_log_or_expr> log_or_expr_ast(log_or_op);
ast<ast_cond_expr> cond_expr_ast(cond_op);


/**** STATEMENTS ****/


ast<ast_var_stm> var_stm_ast(var_stm);
ast<ast_block_stm> block_stm_ast(block_stm);
ast<ast_for_stm> for_stm_ast(for_stm);
ast<ast_while_stm> while_stm_ast(while_stm);
ast<ast_else_if_stm> else_if_stm_ast(else_if_stm);
ast<ast_else_block_stm> else_block_stm_ast(else_block_stm);
ast<ast_if_stm> if_stm_ast(if_stm);
ast<ast_print_stm> print_stm_ast(print_stm);
ast<ast_return_stm> return_stm_ast(return_stm);
ast<ast_assignment_stm> assignment_stm_ast(assignment_stm);
ast<ast_expr_stm> expr_stm_ast(expression_stm);


/**** DECLARATIONS ****/

ast<ast_struct_decl> struct_decl_ast(struct_decl);
ast<ast_var_inst> var_inst_ast(var_inst);
ast<ast_var_init> var_init_ast(var_init);
ast<ast_var_def> var_def_ast(var_def);
ast<ast_var_decl> var_decl_ast(var_decl);
ast<ast_func_decl> func_decl_ast(func_decl);
ast<ast_translation_unit> translation_unit_ast(translation_unit);
