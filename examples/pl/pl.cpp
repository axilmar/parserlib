#include <cstdio>
#include <iostream>
#include "parserlib.hpp"
using namespace std;
using namespace parserlib;


/******************************************************************************
    GRAMMAR
 ******************************************************************************/
 
 
/**** TERMINALS ****/ 
 
 
//any character
#define ANY_CHAR             range(0, 255)


//line end
#define LINE_END             nl(expr("\n\r") | "\r\n" | '\n' | '\r')
 
 
//block comments
#define BLOCK_COMMENTS       ("/*" >> *(!(expr("/*")) >> ANY_CHAR) >> "*/")
 
 
//line comments
#define LINE_COMMENTS        ("//" >> *(!(expr("//")) >> ANY_CHAR) >> (LINE_END | eof()))


//letter
#define LETTER               (range('a', 'z') | range('A', 'Z'))


//digit
#define DIGIT                range('0', '9')


//string character
#define STRING_CHAR          (('\'' >> set("\"'nr0")) | ANY_CHAR)


//whitespace
rule whitespace = *(BLOCK_COMMENTS | LINE_COMMENTS | range(0, 32));


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
rule bool_literal = expr("true") | "false";


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


//argument
extern rule expression;
rule argument = expression;


//function call
rule function_call = identifier >> '(' >> -(argument >> *(',' >> argument)) >> ')';


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
extern rule unary;
rule logical_not_ = '!' >> unary;


//positive
rule positive = '+' >> unary;


//negative
rule negative = '-' >> unary;


//unary
rule unary = logical_not_
           | positive
           | negative
           | value;


//multiplication
extern rule mul;
rule mul_op = '*' >> mul;
rule div_op = '/' >> mul;
rule mul = unary >> -(mul_op | div_op);


//add
extern rule add;
rule add_op = '+' >> add;
rule sub_op = '-' >> add;
rule add = mul >> -(add_op | mul_op);


//logical
extern rule logical;
rule log_and = '&&' >> logical;
rule log_or  = '||' >> logical;
rule logical = add >> -(log_and | log_or);


//conditional
extern rule cond;
rule cond1 = '?' >> cond >> ':' >> cond;
rule cond = logical >> -cond1;


rule expression = cond;
 
 
/**** STATEMENTS ****/ 
 
 
//local variable statement
extern rule var_decl;
extern rule var_stm = var_decl;


//block statement
extern rule statement;
rule block_stm = '{' >> *statement >> '}';


//for statement
extern rule assignment;
rule for_init = var_decl | assignment;
rule for_stm = expr("for") >> '(' >> -for_init >> ';' >> -expression >> -assignment >> ')' >> block_stm;


//while statement
rule while_stm = expr("while") >> '(' >> expression >> ')' >> block_stm;


//if statement
extern rule if_stm;
rule else_stm = "else" >> (block_stm | if_stm);
rule if_stm = expr("if") >> '(' >> expression >> ')' >> block_stm >> -else_stm;


//print statement
rule print_stm = "print" >> expression;


//assignment
rule assignment = expression >> '=' >> expression;


//statement
rule statement = var_stm
               | block_stm
               | for_stm
               | while_stm
               | if_stm
               | print_stm
               | assignment
               | expression
               | ';';


/**** DECLARATIONS ****/ 


//variable
rule var = identifier >> ':' >> type;
 
 
//variable declaration
rule var_decl = 'var' >> var >> -('=' >> expression);


//struct variable
rule struct_var = var;


//struct member
rule struct_member = struct_var
                   | ';';
               
                   
//struct declaration
rule struct_decl = "struct" >> identifier >> '{' >> *struct_member >> '}';


//function declaration
rule func_decl = "func" >> identifier >> '(' >> -(var >> *(',' >> var)) >> ')' >> ':' >> type >> block_stm;
                
                
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
 
 
/***** DECLARATIONS ****/
 
 
//base class for declarations
class ast_declaration : public ast_container {
public:
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
 
 
/**** DECLARATIONS ****/ 
 
 
//translation unit
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
    
    //create a buffer to hold the whole file
    vector<unsigned char> buf(size);
    
    //load it
    fread(&buf[0], 1, size, file);
    
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
    }
    
    //sort and print errors
    errors.sort();
    for(error_list::iterator it = errors.begin();
        it != errors.end();
        ++it)
    {
        error &e = *it;
        cout << "ERROR: line " 
             << e.m_begin.m_line 
             << ", col " 
             << e.m_begin.m_col 
             << ": ";
        wcout << e.m_msg;
        wcout << endl;
    }        

    cout << "finished. Press any key to continue.\n";
    getchar();    
    return 0;
}

