#ifndef PL_AST_HPP
#define PL_AST_HPP


#include <string>
#include "parserlib.hpp"


/******************************************************************************
    AST
 ******************************************************************************/
 
 
/***** TERMINALS ****/


//identifier
class ast_identifier : public parserlib::ast_node {
public:
    ///value.
    std::string m_value;

    //constructs the identifier.
    virtual void construct(parserlib::ast_stack &st);
};


//base class for expressions.
class ast_expr : public parserlib::ast_container {
public:
};


//base class for literals.
class ast_literal : public ast_expr {
public:
};


//float
class ast_float_literal : public ast_literal {
public:
    //value
    double m_value;
    
    //constructs the value.
    virtual void construct(parserlib::ast_stack &st);
};


//integer literal
class ast_int_literal : public ast_literal {
public:
    //value
    int m_value;

    //constructs the value.
    virtual void construct(parserlib::ast_stack &st);
};


//string literal
class ast_string_literal : public ast_literal {
public:
    //value
    std::string m_value;

    //constructs the value.
    virtual void construct(parserlib::ast_stack &st);
};


//character literal
class ast_char_literal : public ast_literal {
public:
    //value
    char m_value;

    //constructs the value.
    virtual void construct(parserlib::ast_stack &st);
};


//boolean literal
class ast_bool_literal : public ast_literal {
public:
    //value
    bool m_value;

    //constructs the value.
    virtual void construct(parserlib::ast_stack &st);
};


/***** TYPES ****/


//base class for types.
class ast_type : public parserlib::ast_container {
public:
};


//void type.
class ast_void_type : public ast_type {
public:    
};


//int type.
class ast_int_type : public ast_type {
public:
};


//float type.
class ast_float_type : public ast_type {
public:
};


//boolean type.
class ast_bool_type : public ast_type {
public:
};


//string type.
class ast_string_type : public ast_type {
public:
};


//char type.
class ast_char_type : public ast_type {
public:
};


//id type.
class ast_id_type : public ast_type {
public:
    //id
    parserlib::ast_ptr<ast_identifier> m_id;
};


/***** EXPRESSIONS ****/


//function call expression
class ast_func_call : public ast_expr {
public:
    //name of the function to call
    parserlib::ast_ptr<ast_identifier> m_name;

    //arguments
    parserlib::ast_list<ast_expr> m_args;
};


//member access
class ast_member_access : public ast_expr {
public:
    //path to member
    parserlib::ast_list<ast_identifier> m_path;
};


//literal value
class ast_literal_value : public ast_expr {
public:
    //literal
    parserlib::ast_ptr<ast_literal> m_literal;
};


//base class for unary expressions
class ast_unary_expr : public ast_expr {
public:
    //the sub-expression
    parserlib::ast_ptr<ast_expr> m_expr;
};


//logical not expression
class ast_log_not_expr : public ast_unary_expr {
public:
};


//positive expression
class ast_positive_expr : public ast_unary_expr {
public:
};


//negative expression
class ast_negative_expr : public ast_unary_expr {
public:
};


//base class for binary expressions
class ast_binary_expr : public ast_expr {
public:
    //left expression
    parserlib::ast_ptr<ast_expr> m_left;

    //right expression
    parserlib::ast_ptr<ast_expr> m_right;
};


//mupliplication expression
class ast_mul_expr : public ast_binary_expr {
public:
};


//division expression
class ast_div_expr : public ast_binary_expr {
public:
};


//addition expression
class ast_add_expr : public ast_binary_expr {
public:
};


//subtraction expression
class ast_sub_expr : public ast_binary_expr {
public:
};


//equals expression
class ast_eq_expr : public ast_binary_expr {
public:
};


//different expression
class ast_diff_expr : public ast_binary_expr {
public:
};


//less-than expression
class ast_lt_expr : public ast_binary_expr {
public:
};


//less-than or equal expression
class ast_lte_expr : public ast_binary_expr {
public:
};


//greater-than expression
class ast_gt_expr : public ast_binary_expr {
public:
};


//greater-than or equal expression
class ast_gte_expr : public ast_binary_expr {
public:
};


//logical or
class ast_log_or_expr : public ast_binary_expr {
public:
};


//logical and
class ast_log_and_expr : public ast_binary_expr {
public:
};


//conditional expression
class ast_cond_expr : public ast_expr {
public:
    //condition
    parserlib::ast_ptr<ast_expr> m_cond;

    //the then part
    parserlib::ast_ptr<ast_expr> m_then;

    //the else part
    parserlib::ast_ptr<ast_expr> m_else;
};


/***** STATEMENTS ****/


//variable instance
class ast_var_inst : public parserlib::ast_container {
public:
    //name
    parserlib::ast_ptr<ast_identifier> m_name;

    //type
    parserlib::ast_ptr<ast_type> m_type;
};


//variable definition
class ast_var_def : public parserlib::ast_container {
public:
    //var instance
    parserlib::ast_ptr<ast_var_inst> m_var_inst;

    //initializer expression
    parserlib::ast_ptr<ast_expr, true> m_init_expr;
};


//base class for statements
class ast_stm : public parserlib::ast_container {
};


//variable statement
class ast_var_stm : public ast_stm {
public:
    //variable
    parserlib::ast_ptr<ast_var_def> m_var;
};


//block statement
class ast_block_stm : public ast_stm {
public:
    //list of statements
    parserlib::ast_list<ast_stm> m_stms;
};


//for statement
class ast_for_stm : public ast_stm {
public:
    //optional initializer statement
    parserlib::ast_ptr<ast_stm, true> m_init_stm;

    //optional expression for condition
    parserlib::ast_ptr<ast_expr, true> m_cond;

    //optional statement for step
    parserlib::ast_ptr<ast_stm, true> m_step;

    //body
    parserlib::ast_ptr<ast_block_stm> m_body;
};


//while statement
class ast_while_stm : public ast_stm {
public:
    //conditional expression
    parserlib::ast_ptr<ast_expr, true> m_cond;

    //body
    parserlib::ast_ptr<ast_block_stm> m_body;
};


//base class for else statements
class ast_else_stm : public ast_stm {
};


//else if statement
class ast_if_stm;
class ast_else_if_stm : public ast_else_stm {
public:
    //if statement
    parserlib::ast_ptr<ast_if_stm> m_if;
};


//else block statement
class ast_else_block_stm : public ast_else_stm {
public:
    //block statement
    parserlib::ast_ptr<ast_block_stm> m_block;
};


//if statement
class ast_if_stm : public ast_stm {
public:
    //conditional expression
    parserlib::ast_ptr<ast_expr, true> m_cond;

    //the then part
    parserlib::ast_ptr<ast_block_stm> m_then;

    //the optional else part
    parserlib::ast_ptr<ast_else_stm, true> m_else;
};


//print statement
class ast_print_stm : public ast_stm {
public:
    //list of expressions to print
    parserlib::ast_list<ast_expr> m_exprs;
};


//reutrn statement
class ast_return_stm : public ast_stm {
public:
    //expression
    parserlib::ast_ptr<ast_expr, true> m_expr;
};


//assignment statement
class ast_assignment_stm : public ast_stm {
public:
    //the lvalue
    parserlib::ast_ptr<ast_expr> m_lvalue;

    //the rvalue
    parserlib::ast_ptr<ast_expr> m_rvalue;
};


//expression statement
class ast_expr_stm : public ast_stm {
public:
    //the expression
    parserlib::ast_ptr<ast_expr> m_expr;
};


/***** DECLARATIONS ****/


//base class for declarations
class ast_declaration : public parserlib::ast_container {
public:
};


//struct declaration
class ast_struct_decl : public ast_declaration {
public:
    //name
    parserlib::ast_ptr<ast_identifier> m_name;

    //list of member variables
    parserlib::ast_list<ast_var_inst> m_member_vars;
};


//variable declaration
class ast_var_decl : public ast_declaration {
public:
    //variable
    parserlib::ast_ptr<ast_var_def> m_var;
};


//function declaration
class ast_func_decl : public ast_declaration {
public:
    //name
    parserlib::ast_ptr<ast_identifier> m_name;

    //list of parameter variables
    parserlib::ast_list<ast_var_inst> m_param_vars;

    //return type
    parserlib::ast_ptr<ast_type> m_ret_type;

    //body
    parserlib::ast_ptr<ast_block_stm> m_body;
};


//unit
class ast_translation_unit : public parserlib::ast_container {
public:
    //declarations
    parserlib::ast_list<ast_declaration> m_declarations;
};    


#endif //PL_AST_HPP
