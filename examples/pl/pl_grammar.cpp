#include "pl_grammar.hpp"
#include "pl_ast.hpp"
using namespace parserlib;


/******************************************************************************
    GRAMMAR
 ******************************************************************************/


/**** TERMINALS ****/


//newline
#define NEWLINE              nl(expr("\r\n") | "\n\r" | '\n' | '\r')


//any character
#define ANY_CHAR             range(0, 255)


//block comments
#define BLOCK_COMMENTS       ("/*" >> *(!expr("*/") >> (NEWLINE | ANY_CHAR)) >> "*/")


//line comments
#define LINE_COMMENTS        ("//" >> *(!(NEWLINE | eof()) >> ANY_CHAR) >> (NEWLINE | eof()))


//letter
#define LETTER               (range('a', 'z') | range('A', 'Z'))


//digit
#define DIGIT                range('0', '9')


//string character
#define STRING_CHAR          (('\'' >> set("\"'nr0")) | (!set("\"'") >> ANY_CHAR))


//whitespace
rule whitespace = *(BLOCK_COMMENTS | LINE_COMMENTS | NEWLINE | range(0, 32));


//identifier
rule identifier = term(LETTER >> *(LETTER | DIGIT | '_'));


//float literal
rule float_literal = term(+DIGIT >> '.' >> +DIGIT >> -(set("eE") >> -set("+-") >> +DIGIT));


//integer literal
rule int_literal = term(+DIGIT);


//string literal
rule string_literal = term('"' >> *STRING_CHAR >> '"');


//char literal
rule char_literal = term('\'' >> STRING_CHAR >> '\'');


//boolean literal
rule bool_literal = term(expr("true") | "false");


/**** TYPES ****/


//void type
rule void_type = "void";


//integer type
rule int_type = "int";


//float type
rule float_type = "float";


//boolean type
rule bool_type = "bool";


//string type
rule string_type = "string";


//character type
rule char_type = "char";


//id type.
rule id_type = identifier;


//type
rule type = int_type
          | float_type
          | bool_type
          | string_type
          | char_type
          | id_type;


/**** EXPRESSIONS ****/


//function call
extern rule expression;
rule function_call = identifier >> '(' >> -(expression >> *(',' >> expression)) >> ')';


//member access
rule member_access = identifier >> *('.' >> identifier);


//literal value
rule literal_value = float_literal
                   | int_literal
                   | string_literal
                   | char_literal
                   | bool_literal;


//value
rule value = function_call
           | member_access
           | literal_value
           | '(' >> expression >> ')';


//logical not
extern rule unary_expr;
rule logical_not_expr = '!' >> unary_expr;


//positive
rule positive_expr = '+' >> unary_expr;


//negative
rule negative_expr = '-' >> unary_expr;


//unary
rule unary_expr = logical_not_expr
                | positive_expr
                | negative_expr
                | value;


//multiplication
extern rule mul_expr;
rule mul_op = '*' >> mul_expr;
rule div_op = '/' >> mul_expr;
rule mul_expr = unary_expr >> -(mul_op | div_op);


//add
extern rule add_expr;
rule add_op = '+' >> add_expr;
rule sub_op = '-' >> add_expr;
rule add_expr = mul_expr >> -(add_op | mul_op);


//compare expression
extern rule cmp_expr;
rule lt_op = "<" >> cmp_expr;
rule lte_op = "<=" >> cmp_expr;
rule gt_op = ">" >> cmp_expr;
rule gte_op = ">=" >> cmp_expr;
rule cmp_expr = add_expr >> -(lt_op | lte_op | gt_op | gte_op);


//equal expression
extern rule eq_expr;
rule eq_op = "==" >> eq_expr;
rule diff_op = "!=" >> eq_expr;
rule eq_expr = cmp_expr >> -(eq_op | diff_op);


//logical
extern rule logical_expr;
rule log_and_op = "&&" >> logical_expr;
rule log_or_op  = "||" >> logical_expr;
rule logical_expr = eq_expr >> -(log_and_op | log_or_op);


//conditional
extern rule cond_expr;
rule cond_op = '?' >> cond_expr >> ':' >> cond_expr;
rule cond_expr = logical_expr >> -cond_op;


//expression
rule expression = cond_expr;


/**** STATEMENTS ****/


//local variable statement
extern rule var_def;
rule var_stm = var_def;


//block statement
extern rule statement;
rule block_stm = '{' >> *statement >> '}';


//for statement
extern rule assignment_stm;
rule for_init = var_stm | assignment_stm;
rule for_stm = expr("for") >> '(' >> -for_init >> ';' >> -expression >> -assignment_stm >> ')' >> block_stm;


//while statement
rule while_stm = expr("while") >> '(' >> expression >> ')' >> block_stm;


//if statement
extern rule if_stm;
rule else_if_stm    = if_stm;
rule else_block_stm = block_stm;
rule if_stm = expr("if") >> '(' >> expression >> ')' >> block_stm >> -("else" >> (else_if_stm | else_block_stm));


//print statement
rule print_stm = "print" >> expression >> *(',' >> expression);


//reutrn statement
rule return_stm = "return" >> -expression;


//assignment statement
rule assignment_stm = expression >> '=' >> expression;


//expression statement
rule expression_stm = expression;


//statement
rule statement = var_stm
               | block_stm
               | for_stm
               | while_stm
               | if_stm
               | print_stm
               | return_stm
               | assignment_stm
               | expression_stm
               | ';';


/**** DECLARATIONS ****/


//variable instance
rule var_inst = identifier >> ':' >> type;


//variable definition
rule var_def = "var" >> var_inst >> -('=' >> expression);


//variable declaration
rule var_decl = var_def;


//struct member
rule struct_member = var_inst
                   | ';';


//struct declaration
rule struct_decl = "struct" >> identifier >> '{' >> *struct_member >> '}';


//function declaration
rule func_decl = "func" >> identifier >> '(' >> -(var_inst >> *(',' >> var_inst)) >> ')' >> ':' >> type >> block_stm;


//declaration
rule declaration = struct_decl
                 | var_decl
                 | func_decl
                 |  ';';


//translation unit
rule translation_unit = *declaration;


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
ast<ast_var_def> var_def_ast(var_def);
ast<ast_var_decl> var_decl_ast(var_decl);
ast<ast_func_decl> func_decl_ast(func_decl);
ast<ast_translation_unit> translation_unit_ast(translation_unit);


