#ifndef PL_AST_HPP
#define PL_AST_HPP


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
};


//base class for expressions.
class ast_expr : public parserlib::ast_container {
public:
    //the type name
    std::string m_type_name;

    //get the type name of the expression
    virtual std::string type_name() const { return m_type_name; }

    //type check the expression
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors) = 0;
    
    //locate the closest variable with the given name
    ast_var_inst *find_var(const std::string &name) const;
};


//base class for literals.
class ast_literal : public ast_expr {
public:
    //no type check for most literals
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors) {}
};


//float
class ast_float_literal : public ast_literal {
public:
    //value
    double m_value;
    
    //error
    bool m_error;
    
    //constructs the value.
    virtual void construct(parserlib::ast_stack &st);

    //get the type name of the expression
    virtual std::string type_name() const { return "float"; }

    //type check the expression
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//integer literal
class ast_int_literal : public ast_literal {
public:
    //value
    int m_value;
    
    //error
    bool m_error;

    //constructs the value.
    virtual void construct(parserlib::ast_stack &st);

    //get the type name of the expression
    virtual std::string type_name() const { return "int"; }

    //type check the expression
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//string literal
class ast_string_literal : public ast_literal {
public:
    //value
    std::string m_value;

    //constructs the value.
    virtual void construct(parserlib::ast_stack &st);
    
    //get the type name of the expression
    virtual std::string type_name() const { return "string"; }
};


//character literal
class ast_char_literal : public ast_literal {
public:
    //value
    char m_value;

    //constructs the value.
    virtual void construct(parserlib::ast_stack &st);
    
    //get the type name of the expression
    virtual std::string type_name() const { return "char"; }
};


//boolean literal
class ast_bool_literal : public ast_literal {
public:
    //value
    bool m_value;

    //constructs the value.
    virtual void construct(parserlib::ast_stack &st);
    
    //get the type name of the expression
    virtual std::string type_name() const { return "bool"; }
};


/***** TYPES ****/


//base class for types.
class ast_type : public parserlib::ast_container {
public:
    //return the type's name
    virtual std::string name() const = 0;
};


//void type.
class ast_void_type : public ast_type {
public:    
    //return the type's name
    virtual std::string name() const { return "void"; }
};


//int type.
class ast_int_type : public ast_type {
public:
    //return the type's name
    virtual std::string name() const { return "int"; }
};


//float type.
class ast_float_type : public ast_type {
public:
    //return the type's name
    virtual std::string name() const { return "float"; }
};


//boolean type.
class ast_bool_type : public ast_type {
public:
    //return the type's name
    virtual std::string name() const { return "bool"; }
};


//string type.
class ast_string_type : public ast_type {
public:
    //return the type's name
    virtual std::string name() const { return "string"; }
};


//char type.
class ast_char_type : public ast_type {
public:
    //return the type's name
    virtual std::string name() const { return "char"; }
};


//id type.
class ast_id_type : public ast_type {
public:
    //id
    parserlib::ast_ptr<ast_identifier> m_id;
    
    //return the type's name
    virtual std::string name() const { return m_id->m_value; }
};


/***** EXPRESSIONS ****/


//function call expression
class ast_func_call : public ast_expr {
public:
    //name of the function to call
    parserlib::ast_ptr<ast_identifier> m_name;

    //arguments
    parserlib::ast_list<ast_expr> m_args;
    
    //typename
    std::string m_type_name;
    
    //get the type name of the expression
    virtual std::string type_name() const { return m_type_name; }

    //type check the expression
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//member access
class ast_member_access : public ast_expr {
public:
    //path to member
    parserlib::ast_list<ast_identifier> m_path;

    //get the type name of the expression
    virtual std::string type_name() const;

    //type check the expression
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
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
    //get the type name of the expression
    virtual std::string type_name() const;

    //type check the expression
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//positive expression
class ast_positive_expr : public ast_unary_expr {
public:
    //get the type name of the expression
    virtual std::string type_name() const;

    //type check the expression
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//negative expression
class ast_negative_expr : public ast_unary_expr {
public:
    //get the type name of the expression
    virtual std::string type_name() const;

    //type check the expression
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//base class for binary expressions
class ast_binary_expr : public ast_expr {
public:
    //left expression
    parserlib::ast_ptr<ast_expr> m_left;

    //right expression
    parserlib::ast_ptr<ast_expr> m_right;
    
    //get the type name of the expression
    virtual std::string type_name() const;
};


//mupliplication expression
class ast_mul_expr : public ast_binary_expr {
public:
    //get the type name of the expression
    virtual std::string type_name() const;

    //type check the expression
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//division expression
class ast_div_expr : public ast_binary_expr {
public:
    //get the type name of the expression
    virtual std::string type_name() const;

    //type check the expression
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//addition expression
class ast_add_expr : public ast_binary_expr {
public:
    //get the type name of the expression
    virtual std::string type_name() const;

    //type check the expression
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//subtraction expression
class ast_sub_expr : public ast_binary_expr {
public:
    //get the type name of the expression
    virtual std::string type_name() const;

    //type check the expression
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//base class for boolean expressions
class ast_bool_expr : public ast_binary_expr {
public:
    //get the type name of the expression
    virtual std::string type_name() const { return "bool"; }
};    
    

//equals expression
class ast_eq_expr : public ast_bool_expr {
public:
    //type check the expression
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//different expression
class ast_diff_expr : public ast_bool_expr {
public:
    //type check the expression
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//less-than expression
class ast_lt_expr : public ast_bool_expr {
public:
    //type check the expression
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//less-than or equal expression
class ast_lte_expr : public ast_bool_expr {
public:
    //type check the expression
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//greater-than expression
class ast_gt_expr : public ast_bool_expr {
public:
    //type check the expression
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//greater-than or equal expression
class ast_gte_expr : public ast_bool_expr {
public:
    //type check the expression
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//logical or
class ast_log_or_expr : public ast_bool_expr {
public:
    //type check the expression
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//logical and
class ast_log_and_expr : public ast_bool_expr {
public:
    //type check the expression
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
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

    //get the type name of the expression
    virtual std::string type_name() const;

    //type check the expression
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


/***** STATEMENTS ****/


//base class for statements
class ast_stm : public parserlib::ast_container {
public:
    //typecheck.
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors) = 0;
};


//variable statement
class ast_var_stm : public ast_stm {
public:
    //variable
    parserlib::ast_ptr<ast_var_def> m_var_def;

    //typecheck.
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//block statement
class ast_block_stm : public ast_stm {
public:
    //list of statements
    parserlib::ast_list<ast_stm> m_stms;

    //typecheck.
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//for init
class ast_for_init : public parserlib::ast_container {
public:
    //init statement
    parserlib::ast_ptr<ast_stm> m_stm;

    //typecheck.
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//for conditional
class ast_for_cond : public parserlib::ast_container {
public:
    //expression
    parserlib::ast_ptr<ast_expr> m_expr;

    //typecheck.
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//for step
class ast_for_step : public parserlib::ast_container {
public:
    //assignment statement
    parserlib::ast_ptr<ast_assignment_stm> m_assignment;

    //typecheck.
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
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

    //typecheck.
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//while statement
class ast_while_stm : public ast_stm {
public:
    //conditional expression
    parserlib::ast_ptr<ast_expr, true> m_cond;

    //body
    parserlib::ast_ptr<ast_block_stm> m_body;
    
    //typecheck.
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
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

    //typecheck.
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//else block statement
class ast_else_block_stm : public ast_else_stm {
public:
    //block statement
    parserlib::ast_ptr<ast_block_stm> m_block;

    //typecheck.
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
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

    //typecheck.
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//print statement
class ast_print_stm : public ast_stm {
public:
    //list of expressions to print
    parserlib::ast_list<ast_expr> m_exprs;

    //typecheck.
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//reutrn statement
class ast_return_stm : public ast_stm {
public:
    //expression
    parserlib::ast_ptr<ast_expr, true> m_expr;

    //typecheck.
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//assignment statement
class ast_assignment_stm : public ast_stm {
public:
    //the lvalue
    parserlib::ast_ptr<ast_expr> m_left;

    //the rvalue
    parserlib::ast_ptr<ast_expr> m_right;

    //typecheck.
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//expression statement
class ast_expr_stm : public ast_stm {
public:
    //the expression
    parserlib::ast_ptr<ast_expr> m_expr;

    //typecheck.
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


/***** DECLARATIONS ****/


//base class for declarations
class ast_declaration : public parserlib::ast_container {
public:
    //check if the declaration is type.
    virtual bool is_type() const = 0;
    
    //get the name of the declaration
    virtual std::string name() const = 0;

    //typecheck the code.
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors) = 0;
};


//struct declaration
class ast_struct_decl : public ast_declaration {
public:
    //name
    parserlib::ast_ptr<ast_identifier> m_name;

    //list of member variables
    parserlib::ast_list<ast_var_inst> m_member_vars;

    //check if the declaration is type.
    virtual bool is_type() const { return true; }
    
    //get the name of the declaration
    virtual std::string name() const { return m_name->m_value; }

    //typecheck the code.
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
    
    //find member variable
    ast_var_inst *find_member_var(const std::string &name) const;
};


//variable instance
class ast_var_inst : public parserlib::ast_container {
public:
    //name
    parserlib::ast_ptr<ast_identifier> m_name;

    //type
    parserlib::ast_ptr<ast_type> m_type;
    
    //type check
    void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//variable initializer
class ast_var_init : public parserlib::ast_container {
public:
    //expression.
    parserlib::ast_ptr<ast_expr> m_expr;
    
    //type check
    void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//variable definition
class ast_var_def : public parserlib::ast_container {
public:
    //var instance
    parserlib::ast_ptr<ast_var_inst> m_var_inst;

    //initializer expression
    parserlib::ast_ptr<ast_var_init, true> m_init;

    //type check
    void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//variable declaration
class ast_var_decl : public ast_declaration {
public:
    //variable
    parserlib::ast_ptr<ast_var_def> m_var_def;

    //check if the declaration is type.
    virtual bool is_type() const { return false; }
    
    //get the name of the declaration
    virtual std::string name() const { return m_var_def->m_var_inst->m_name->m_value; }

    //typecheck the code.
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
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

    //check if the declaration is type.
    virtual bool is_type() const { return false; }
    
    //get the name of the declaration
    virtual std::string name() const { return m_name->m_value; }

    //typecheck the code.
    virtual void type_check(ast_translation_unit *unit, parserlib::error_list &errors);
};


//unit
class ast_translation_unit : public parserlib::ast_container {
public:
    //declarations
    parserlib::ast_list<ast_declaration> m_declarations;
    
    //type check the code
    void type_check(parserlib::error_list &errors);
    
    //find type
    std::string find_type(const std::string &name) const;
    
    //check if the given type exists; if not, add an error to the error list.
    void check_type(ast_type *type, parserlib::error_list &errors) const;
    
    //find a declaration by name
    ast_declaration *find_decl(const std::string &name) const;
};    


#endif //PL_AST_HPP
