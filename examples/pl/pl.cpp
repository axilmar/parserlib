#include <cstdio>
#include <iostream>
#include <sstream>
#include "parserlib.hpp"
using namespace std;
using namespace parserlib;


/******************************************************************************
    GRAMMAR
 ******************************************************************************/
 
 
/**** TERMINALS ****/ 
 
 
//newline
#define NEWLINE              nl(expr("\r\n") | '\n' | '\r')
 
 
//any character
#define ANY_CHAR             range(0, 255)


//block comments
#define BLOCK_COMMENTS       ("/*" >> *(!expr("/*") >> (NEWLINE | ANY_CHAR)) >> "*/")
 
 
//line comments
#define LINE_COMMENTS        ("//" >> *(!expr("//") >> !NEWLINE >> ANY_CHAR) >> (NEWLINE | eof()))


//letter
#define LETTER               (range('a', 'z') | range('A', 'Z'))


//digit
#define DIGIT                range('0', '9')


//string character
#define STRING_CHAR          (('\'' >> set("\"'nr0")) | ANY_CHAR)


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


//logical
extern rule logical_expr;
rule log_and_op = '&&' >> logical_expr;
rule log_or_op  = '||' >> logical_expr;
rule logical_expr = add_expr >> -(log_and_op | log_or_op);


//conditional
extern rule cond_expr;
rule cond_op = '?' >> cond_expr >> ':' >> cond_expr;
rule cond_expr = logical_expr >> -cond_op;


//expression
rule expression = cond_expr;
 
 
/**** STATEMENTS ****/ 
 
 
//local variable statement
extern rule var_def;
extern rule var_stm = var_def;


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
rule print_stm = "print" >> expression >> -(',' >> expression);


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
    AST
 ******************************************************************************/
 
 
/***** TERMINALS ****/


//identifier
class ast_identifier : public ast_node {
public:
    ///value.
    string m_value;
    
    //constructs the identifier.
    virtual void construct(const pos &b, const pos &e, ast_stack &st) {
        for(input::const_iterator it = b.m_it; it != e.m_it; ++it) {
            m_value += (char)*it;
        }
    }
};


//base class for literals.
class ast_literal : public ast_node {
public:
};


//float
class ast_float_literal : public ast_literal {
public:
    //value
    double m_value;
    
    //constructs the value.
    virtual void construct(const pos &b, const pos &e, ast_stack &st) {
        stringstream stream;
        for(input::const_iterator it = b.m_it; it != e.m_it; ++it) {
            stream << (char)*it;
        }
        stream >> m_value;
    }
};


//integer literal
class ast_int_literal : public ast_literal {
public:
    //value
    int m_value;
    
    //constructs the value.
    virtual void construct(const pos &b, const pos &e, ast_stack &st) {
        stringstream stream;
        for(input::const_iterator it = b.m_it; it != e.m_it; ++it) {
            stream << (char)*it;
        }
        stream >> m_value;
    }
};


//string literal
class ast_string_literal : public ast_literal {
public:
    //value
    string m_value;
    
    //constructs the value.
    virtual void construct(const pos &b, const pos &e, ast_stack &st) {
        stringstream stream;
        for(input::const_iterator it = b.m_it; it != e.m_it; ++it) {
            stream << (char)*it;
        }
        stream >> m_value;
    }
};
 
 
//character literal
class ast_char_literal : public ast_literal {
public:
    //value
    char m_value;
    
    //constructs the value.
    virtual void construct(const pos &b, const pos &e, ast_stack &st) {
        stringstream stream;
        for(input::const_iterator it = b.m_it; it != e.m_it; ++it) {
            stream << (char)*it;
        }
        stream >> m_value;
    }
};
 
 
//boolean literal
class ast_bool_literal : public ast_literal {
public:
    //value
    bool m_value;
    
    //constructs the value.
    virtual void construct(const pos &b, const pos &e, ast_stack &st) {
        stringstream stream;
        for(input::const_iterator it = b.m_it; it != e.m_it; ++it) {
            stream << (char)*it;
        }
        string v;
        stream >> v;
        m_value = v == "true";
    }
};
 
 
/***** TYPES ****/


//base class for types.
class ast_type : public ast_node {
};


//int type.
class ast_int_type : public ast_type {
public:
    //does nothing. Types do not have a value.
    virtual void construct(const pos &b, const pos &e, ast_stack &st) {}
};
 
 
//float type.
class ast_float_type : public ast_type {
public:
    //does nothing. Types do not have a value.
    virtual void construct(const pos &b, const pos &e, ast_stack &st) {}
};
 
 
//boolean type.
class ast_bool_type : public ast_type {
public:
    //does nothing. Types do not have a value.
    virtual void construct(const pos &b, const pos &e, ast_stack &st) {}
};
 
 
//string type.
class ast_string_type : public ast_type {
public:
    //does nothing. Types do not have a value.
    virtual void construct(const pos &b, const pos &e, ast_stack &st) {}
};
 
 
//char type.
class ast_char_type : public ast_type {
public:
    //does nothing. Types do not have a value.
    virtual void construct(const pos &b, const pos &e, ast_stack &st) {}
};
 
 
//id type.
class ast_id_type : public ast_type {
public:
    //value
    string m_value;
    
    //constructs the value.
    virtual void construct(const pos &b, const pos &e, ast_stack &st) {
        stringstream stream;
        for(input::const_iterator it = b.m_it; it != e.m_it; ++it) {
            stream << (char)*it;
        }
        stream >> m_value;
    }
};
 
 
/***** EXPRESSIONS ****/


//base class for expressions.
class ast_expr : public ast_container {
};


//function call expression
class ast_func_call : public ast_expr {
public:
    //name of the function to call
    ast_ptr<ast_identifier> m_name;
    
    //arguments
    ast_list<ast_expr> m_args;
};


//member access
class ast_member_access : public ast_expr {
public:
    //path to member
    ast_list<ast_identifier> m_path;
};


//literal value
class ast_literal_value : public ast_expr {
public:
    //literal
    ast_ptr<ast_literal> m_literal;
};


//base class for unary expressions
class ast_unary_expr : public ast_expr {
public:
    //the sub-expression
    ast_ptr<ast_expr> m_expr;
};


//logical not expression
class ast_log_not_expr : public ast_unary_expr {
};
 
 
//positive expression
class ast_positive_expr : public ast_unary_expr {
};
 
 
//negative expression
class ast_negative_expr : public ast_unary_expr {
};


//base class for binary expressions
class ast_binary_expr : public ast_expr {
public:
    //left expression
    ast_ptr<ast_expr> m_left;

    //right expression
    ast_ptr<ast_expr> m_right;
};


//mupliplication expression
class ast_mul_expr : public ast_binary_expr {
};
 
 
//division expression
class ast_div_expr : public ast_binary_expr {
};
 
 
//addition expression
class ast_add_expr : public ast_binary_expr {
};
 
 
//subtraction expression
class ast_sub_expr : public ast_binary_expr {
};
 
 
//logical or
class ast_log_or_expr : public ast_binary_expr {
};
 
 
//logical and
class ast_log_and_expr : public ast_binary_expr {
};


//conditional expression
class ast_cond_expr : public ast_expr {
public:
    //condition
    ast_ptr<ast_expr> m_cond;
    
    //the then part
    ast_ptr<ast_expr> m_then;
    
    //the else part
    ast_ptr<ast_expr> m_else;
};
 
 
/***** STATEMENTS ****/


//variable instance
class ast_var_inst : public ast_container {
public:
    //name
    ast_ptr<ast_identifier> m_name;
    
    //type
    ast_ptr<ast_type> m_type;
};


//variable definition
class ast_var_def : public ast_var_inst {
public:
    //initializer expression
    ast_ptr<ast_expr, true> m_init_expr;
};


//base class for statements
class ast_stm : public ast_container {
};


//variable statement
class ast_var_stm : public ast_stm {
public:
    //variable
    ast_ptr<ast_var_def> m_var;
};


//block statement
class ast_block_stm : public ast_stm {
public:
    //list of statements
    ast_list<ast_stm> m_stms;
};


//for statement
class ast_for_stm : public ast_stm {
public:
    //optional initializer statement
    ast_ptr<ast_stm, true> m_init_stm;

    //optional expression for condition
    ast_ptr<ast_expr, true> m_cond;

    //optional statement for step
    ast_ptr<ast_stm, true> m_step;

    //body
    ast_ptr<ast_block_stm> m_body;
};


//while statement
class ast_while_stm : public ast_stm {
public:
    //conditional expression
    ast_ptr<ast_expr, true> m_cond;

    //body
    ast_ptr<ast_block_stm> m_body;
};


//base class for else statements
class ast_else_stm : public ast_stm {
};


//else if statement
class ast_if_stm;
class ast_else_if_stm : public ast_else_stm {
public:
    //if statement
    ast_ptr<ast_if_stm> m_if;
};
 
 
//else block statement
class ast_else_block_stm : public ast_else_stm {
public:
    //block statement
    ast_ptr<ast_block_stm> m_block;
};
 
 
//if statement
class ast_if_stm : public ast_stm {
public:
    //conditional expression
    ast_ptr<ast_expr, true> m_cond;

    //the then part
    ast_ptr<ast_block_stm> m_then;

    //the optional else part
    ast_ptr<ast_else_stm, true> m_else;
};


//print statement
class ast_print_stm : public ast_stm {
public:
    //list of expressions to print
    ast_list<ast_expr> m_exprs;
};


//reutrn statement
class ast_return_stm : public ast_stm {
public:
    //expression
    ast_ptr<ast_expr, true> m_expr;
};


//assignment statement
class ast_assignment_stm : public ast_stm {
public:
    //the lvalue
    ast_ptr<ast_expr> m_lvalue;

    //the rvalue
    ast_ptr<ast_expr> m_rvalue;
};
 
 
//expression statement
class ast_expr_stm : public ast_stm {
public:
    //the expression
    ast_ptr<ast_expr> m_expr;
}; 
 
 
/***** DECLARATIONS ****/


//base class for declarations
class ast_declaration : public ast_container {
public:
}; 


//variable declaration
class ast_var_decl : public ast_declaration {
public:
    //variable
    ast_ptr<ast_var_def> m_var;
};


//struct declaration
class ast_struct_decl : public ast_declaration {
public:
    //name
    ast_ptr<ast_identifier> m_name;
    
    //list of member variables
    ast_list<ast_var_inst> m_member_vars;
};


//function declaration
class ast_func_decl : public ast_declaration {
public:
    //name
    ast_ptr<ast_identifier> m_name;
    
    //list of parameter variables
    ast_list<ast_var_inst> m_param_vars;
    
    //return type
    ast_ptr<ast_type> m_ret_type;
    
    //body
    ast_ptr<ast_block_stm> m_body;
};
 
 
//unit
class ast_translation_unit : public ast_container {
public:
    //declarations
    ast_list<ast_declaration> declarations;
}; 


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
 
 
ast<ast_var_decl> var_decl_ast(var_decl); 
ast<ast_struct_decl> struct_decl_ast(struct_decl); 
ast<ast_func_decl> func_decl_ast(func_decl); 
ast<ast_translation_unit> translation_unit_ast(translation_unit); 


/******************************************************************************
    MAIN
 ******************************************************************************/
 
 
//load a text file into memory
static bool load_text_file(const char *filename, input &i) {
    //open file    
    FILE *file = fopen(filename, "rt");
    if (!file) return false;
    
    //find its size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    //create a buffer to hold the whole file
    vector<unsigned char> buf(size);
    
    //load it
    if (size > 0) {
        fread(&buf[0], 1, size, file);
    }
    
    //test for error
    bool ok = ferror(file) == 0;
    
    //close the file
    fclose(file);
    
    //check for error
    if (!ok) return false;
    
    //convert the file to the input
    i.insert(i.end(), buf.begin(), buf.end());
    
    //success
    return true;
}


//print help
void print_help() {
    cout << "PL compiler 1.0. Syntax:\n";
    cout << "pl <source file name>\n";
}


//main
int main(int argc, char *argv[]) {
    //if no file is specified, print the help
    if (argc < 2) {
        print_help();
        return 0;
    }
    
    //load the file
    input i;
    if (!load_text_file(argv[1], i)) {
        cout << "ERROR: file " << argv[1] << " could not be loaded\n";
        return 0;
    }
    
    cout << "compiling file " << argv[1] << "..." << endl;
    
    //parse the file
    error_list errors;
    ast_translation_unit *ast;
    parse(i, translation_unit, whitespace, errors, ast);
    
    //process ast tree
    if (ast) {
        delete ast;
        cout << "no errors found\n";
    }
    
    //else sort and print errors
    else {
        cout << "found " << errors.size() << " " << (errors.size() > 1 ? "errors" : "error") << ":\n";
        errors.sort();    
        for(error_list::iterator it = errors.begin();
            it != errors.end();
            ++it)
        {
            error &e = *it;
            cout << "    line " 
                 << e.m_begin.m_line 
                 << ", col " 
                 << e.m_begin.m_col 
                 << ": ";
            wcout << e.m_msg;
            wcout << endl;
        }      
    }  

    cout << "finished. Press any key to continue.\n";
    getchar();    
    return 0;
}

