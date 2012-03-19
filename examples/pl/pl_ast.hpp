#ifndef PL_AST_HPP
#define PL_AST_HPP


#include <stdio.h>
#include <string>
#include "parserlib.hpp"


class ast_assignment_stm;
class ast_var_inst;
class ast_var_def;
class ast_translation_unit;


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

    //emit code
    void emit_code(FILE *file, int ident);
};


//base class for expressions.
class ast_expr : public parserlib::ast_container {
public:
    //emit code
    virtual void emit_code(FILE *file, int ident) = 0;
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

    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//integer literal
class ast_int_literal : public ast_literal {
public:
    //value
    int m_value;
    
    //constructs the value.
    virtual void construct(parserlib::ast_stack &st);

    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//string literal
class ast_string_literal : public ast_literal {
public:
    //value
    std::string m_value;

    //constructs the value.
    virtual void construct(parserlib::ast_stack &st);

    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//character literal
class ast_char_literal : public ast_literal {
public:
    //value
    char m_value;

    //constructs the value.
    virtual void construct(parserlib::ast_stack &st);

    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//boolean literal
class ast_bool_literal : public ast_literal {
public:
    //value
    bool m_value;

    //constructs the value.
    virtual void construct(parserlib::ast_stack &st);

    //emit code
    virtual void emit_code(FILE *file, int ident);
};


/***** TYPES ****/


//base class for types.
class ast_type : public parserlib::ast_container {
public:
    //get the type's name
    virtual std::string name() const = 0;
};


//void type.
class ast_void_type : public ast_type {
public:    
    //get the type's name
    virtual std::string name() const { return "void"; }
};


//int type.
class ast_int_type : public ast_type {
public:
    //get the type's name
    virtual std::string name() const { return "int"; }
};


//float type.
class ast_float_type : public ast_type {
public:
    //get the type's name
    virtual std::string name() const { return "double"; }
};


//boolean type.
class ast_bool_type : public ast_type {
public:
    //get the type's name
    virtual std::string name() const { return "bool"; }
};


//string type.
class ast_string_type : public ast_type {
public:
    //get the type's name
    virtual std::string name() const { return "string"; }
};


//char type.
class ast_char_type : public ast_type {
public:
    //get the type's name
    virtual std::string name() const { return "char"; }
};


//id type.
class ast_id_type : public ast_type {
public:
    //id
    parserlib::ast_ptr<ast_identifier> m_id;
    
    //get the type's name
    virtual std::string name() const { return m_id->m_value; }
};


/***** EXPRESSIONS ****/


//begin parenthesis
class ast_begin_paren : public parserlib::ast_node {};


//end parenthesis
class ast_end_paren : public parserlib::ast_node {};


//function call expression
class ast_func_call : public ast_expr {
public:
    //name of the function to call
    parserlib::ast_ptr<ast_identifier> m_name;
    
    //begin parenthesis
    parserlib::ast_ptr<ast_begin_paren> m_begin_paren;

    //arguments
    parserlib::ast_list<ast_expr> m_args;    

    //end parenthesis
    parserlib::ast_ptr<ast_end_paren> m_end_paren;

    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//member access
class ast_member_access : public ast_expr {
public:
    //path to member
    parserlib::ast_list<ast_identifier> m_path;

    //emit code
    virtual void emit_code(FILE *file, int ident);
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
    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//positive expression
class ast_positive_expr : public ast_unary_expr {
public:
    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//negative expression
class ast_negative_expr : public ast_unary_expr {
public:
    //emit code
    virtual void emit_code(FILE *file, int ident);
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
    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//division expression
class ast_div_expr : public ast_binary_expr {
public:
    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//addition expression
class ast_add_expr : public ast_binary_expr {
public:
    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//subtraction expression
class ast_sub_expr : public ast_binary_expr {
public:
    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//base class for boolean expressions
class ast_bool_expr : public ast_binary_expr {
public:
};    
    

//equals expression
class ast_eq_expr : public ast_bool_expr {
public:
    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//different expression
class ast_diff_expr : public ast_bool_expr {
public:
    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//less-than expression
class ast_lt_expr : public ast_bool_expr {
public:
    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//less-than or equal expression
class ast_lte_expr : public ast_bool_expr {
public:
    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//greater-than expression
class ast_gt_expr : public ast_bool_expr {
public:
    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//greater-than or equal expression
class ast_gte_expr : public ast_bool_expr {
public:
    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//logical or
class ast_log_or_expr : public ast_bool_expr {
public:
    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//logical and
class ast_log_and_expr : public ast_bool_expr {
public:
    //emit code
    virtual void emit_code(FILE *file, int ident);
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
    //emit code
    virtual void emit_code(FILE *file, int ident);
};


/***** STATEMENTS ****/


//base class for statements
class ast_stm : public parserlib::ast_container {
public:
    //emit code
    virtual void emit_code(FILE *file, int ident) = 0;
};


//variable statement
class ast_var_stm : public ast_stm {
public:
    //variable
    parserlib::ast_ptr<ast_var_def> m_var_def;

    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//begin block
class ast_block_begin : public parserlib::ast_node {};


//end block
class ast_block_end : public parserlib::ast_node {};


//block statement
class ast_block_stm : public ast_stm {
public:
    //begin block
    parserlib::ast_ptr<ast_block_begin> m_block_begin;

    //list of statements
    parserlib::ast_list<ast_stm> m_stms;

    //end block
    parserlib::ast_ptr<ast_block_end> m_block_end;

    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//for init
class ast_for_init : public parserlib::ast_container {
public:
    //init statement
    parserlib::ast_ptr<ast_stm> m_stm;

    //emit code
    void emit_code(FILE *file, int ident);
};


//for conditional
class ast_for_cond : public parserlib::ast_container {
public:
    //expression
    parserlib::ast_ptr<ast_expr> m_expr;

    //emit code
    void emit_code(FILE *file, int ident);
};


//for step
class ast_for_step : public parserlib::ast_container {
public:
    //assignment statement
    parserlib::ast_ptr<ast_assignment_stm> m_assignment;

    //emit code
    void emit_code(FILE *file, int ident);
};


//for statement
class ast_for_stm : public ast_stm {
public:
    //optional initializer statement
    parserlib::ast_ptr<ast_for_init, true> m_init;

    //optional expression for condition
    parserlib::ast_ptr<ast_for_cond, true> m_cond;

    //optional statement for step
    parserlib::ast_ptr<ast_for_step, true> m_step;

    //body
    parserlib::ast_ptr<ast_block_stm> m_body;

    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//while statement
class ast_while_stm : public ast_stm {
public:
    //conditional expression
    parserlib::ast_ptr<ast_expr, true> m_cond;

    //body
    parserlib::ast_ptr<ast_block_stm> m_body;

    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//base class for else statements
class ast_else_stm : public ast_stm {
public:
};


//else if statement
class ast_if_stm;
class ast_else_if_stm : public ast_else_stm {
public:
    //if statement
    parserlib::ast_ptr<ast_if_stm> m_if;

    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//else block statement
class ast_else_block_stm : public ast_else_stm {
public:
    //block statement
    parserlib::ast_ptr<ast_block_stm> m_block;

    //emit code
    virtual void emit_code(FILE *file, int ident);
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

    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//print statement
class ast_print_stm : public ast_stm {
public:
    //list of expressions to print
    parserlib::ast_list<ast_expr> m_exprs;

    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//reutrn statement
class ast_return_stm : public ast_stm {
public:
    //expression
    parserlib::ast_ptr<ast_expr, true> m_expr;

    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//assignment statement
class ast_assignment_stm : public ast_stm {
public:
    //the lvalue
    parserlib::ast_ptr<ast_expr> m_left;

    //the rvalue
    parserlib::ast_ptr<ast_expr> m_right;

    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//expression statement
class ast_expr_stm : public ast_stm {
public:
    //the expression
    parserlib::ast_ptr<ast_expr> m_expr;

    //emit code
    virtual void emit_code(FILE *file, int ident);
};


/***** DECLARATIONS ****/


//base class for declarations
class ast_declaration : public parserlib::ast_container {
public:
    //emit code
    virtual void emit_code(FILE *file, int ident) = 0;
};


//struct declaration
class ast_struct_decl : public ast_declaration {
public:
    //name
    parserlib::ast_ptr<ast_identifier> m_name;

    //list of member variables
    parserlib::ast_list<ast_var_inst> m_member_vars;

    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//variable instance
class ast_var_inst : public parserlib::ast_container {
public:
    //name
    parserlib::ast_ptr<ast_identifier> m_name;

    //type
    parserlib::ast_ptr<ast_type> m_type;

    //emit code
    void emit_code(FILE *file, int ident);
};


//variable initializer
class ast_var_init : public parserlib::ast_container {
public:
    //expression.
    parserlib::ast_ptr<ast_expr> m_expr;

    //emit code
    void emit_code(FILE *file, int ident);
};


//variable definition
class ast_var_def : public parserlib::ast_container {
public:
    //var instance
    parserlib::ast_ptr<ast_var_inst> m_var_inst;

    //initializer expression
    parserlib::ast_ptr<ast_var_init, true> m_init;

    //emit code
    void emit_code(FILE *file, int ident);
};


//variable declaration
class ast_var_decl : public ast_declaration {
public:
    //variable
    parserlib::ast_ptr<ast_var_def> m_var_def;

    //emit code
    virtual void emit_code(FILE *file, int ident);
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

    //emit code
    virtual void emit_code(FILE *file, int ident);
};


//unit
class ast_translation_unit : public parserlib::ast_container {
public:
    //declarations
    parserlib::ast_list<ast_declaration> m_declarations;    
    
    //emit code
    void emit_code(FILE *file, int ident);
};    


#endif //PL_AST_HPP
