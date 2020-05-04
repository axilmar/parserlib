#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP


#include "parserlib.hpp"


extern parserlib::rule whitespace;
extern parserlib::rule identifier;
extern parserlib::rule float_literal;
extern parserlib::rule int_literal;
extern parserlib::rule string_literal;
extern parserlib::rule char_literal;
extern parserlib::rule bool_literal;
extern parserlib::rule void_type;
extern parserlib::rule int_type;
extern parserlib::rule float_type;
extern parserlib::rule bool_type;
extern parserlib::rule string_type;
extern parserlib::rule char_type;
extern parserlib::rule id_type;
extern parserlib::rule begin_paren;
extern parserlib::rule end_paren;
extern parserlib::rule function_call;
extern parserlib::rule member_access;
extern parserlib::rule logical_not_expr;
extern parserlib::rule positive_expr;
extern parserlib::rule negative_expr;
extern parserlib::rule mul_op;
extern parserlib::rule div_op;
extern parserlib::rule add_op;
extern parserlib::rule sub_op;
extern parserlib::rule eq_op;
extern parserlib::rule diff_op;
extern parserlib::rule lt_op;
extern parserlib::rule lte_op;
extern parserlib::rule gt_op;
extern parserlib::rule gte_op;
extern parserlib::rule log_and_op;
extern parserlib::rule log_or_op;
extern parserlib::rule cond_op;
extern parserlib::rule var_stm;
extern parserlib::rule block_begin;
extern parserlib::rule block_end;
extern parserlib::rule block_stm;
extern parserlib::rule for_init;
extern parserlib::rule for_cond;
extern parserlib::rule for_step;
extern parserlib::rule for_stm;
extern parserlib::rule while_stm;
extern parserlib::rule else_if_stm;
extern parserlib::rule else_block_stm;
extern parserlib::rule if_stm;
extern parserlib::rule print_stm;
extern parserlib::rule return_stm;
extern parserlib::rule assignment_stm;
extern parserlib::rule expression_stm;
extern parserlib::rule struct_decl;
extern parserlib::rule var_inst;
extern parserlib::rule var_init;
extern parserlib::rule var_def;
extern parserlib::rule var_decl;
extern parserlib::rule func_decl;
extern parserlib::rule translation_unit;


#endif //GRAMMAR_HPP
