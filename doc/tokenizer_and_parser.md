# Parserlib

## Tokenizer and parser

* [Using the same library for tokenization and parsing](#using-the-same-library-for-tokenization-and-parsing)
* [Example: calculator](#example:-calculator)
	* [The tokenizer](#the-tokenizer)
		* [The tokenizer grammar](#the-tokenizer-grammar)
		* [Using the tokenizer](#using-the-tokenizer)
	* [The parser](#the-parser)
		* [The parser grammar](#the-parser-grammar)
		* [Using the parser](#using-the-parser)
	* [Tokenizing and parsing with one function call](#tokenizing-and-parsing-with-one-function-call)
	* [Computing the result](#computing-the-result)

### Using the same library for tokenization and parsing

The library can be used both for tokenization and parsing.

The matches produced by the tokenization phase can be fed as input to the parsing phase.

Let's see a calculator example with tokenization and parsing.

### Example: calculator

#### The tokenizer

A calculator must support the following tokens:

- spaces.
- floating point numbers with optional scientific notation.
-  operators: +, -, *, /.
-  parentheses.

Invalid characters must also be recognized and reported as errors.

##### The tokenizer grammar

The grammar for the tokenizer can be:

```cpp
//token ids
enum TOKEN_ID {
	TK_NUM,
    TK_ADD,
    TK_SUB,
    TK_MUL,
    TK_DIV,
    TK_LPAR,
    TK_RPAR
};

//error ids
enum ERR_ID {
    ERR_INVALID_CHARS
};

//whitespace
const auto whitespace = terminal(' ');

//floating point number
const auto digit = range('0', '9');
const auto sign = terminal('+') | '-';
const auto integer = +digit;
const auto exp = terminal('e') | 'E';
const auto tk_num = (-sign >> integer >> -('.' >> integer) >> -(exp >> -sign >> integer))->*TK_NUM;

//operators
const tk_add = terminal('+')->*TK_ADD;
const tk_sub = terminal('-')->*TK_SUB;
const tk_mul = terminal('*')->*TK_MUL;
const tk_div = terminal('/')->*TK_DIV;

//parentheses
const tk_lpar = terminal('(')->*TK_LPAR;
const tk_rpar = terminal(')')->*TK_RPAR;

//token
const auto token
    = tk_num
    | tk_add
    | tk_sub
    | tk_mul
    | tk_div
    | tk_lpar
    | tk_rpar
    ;

//recognized symbol
const auto valid_tk_symbol = whitespace | token;

//the error handler; skip to next valid symbol
const auto err_invalid_chars = error(ERR_INVALID_CHARS, skip_before(valid_tk_symbol));

//finally, the tokenizer; must parse the whole input
const auto tokenizer = *(valid_tk_symbol | err_invalid_chars) >> end();
```

##### Using the tokenizer

Straight to the code:

```cpp
std::string input = "(1 + (2 * 3.5) - -23.5e+10)";
parse_context<> tokenizer_pc(input);
tokenizer.parse(tokenizer_pc);
```

The above produces a list of the following matches for the parser:
```
TK_LPAR, TK_NUM(1), TK_ADD, TK_LPAR, TK_NUM(2), TK_MUL, TK_NUM(3.5), TK_RPAR, TK_SUB, TK_NUM(-23.5e+10), TK_RPAR
```

#### The parser

The parser needs to do the following things:

- provide left associativity
- implement the operator precedence is this order: *, /, +, -.

##### The parser grammar

The grammar will be over tokens instead of characters.

Therefore, we need to define a custom parse context that parses tokens.

Here is the parser code:

```cpp
//the parse context for the parser
using parser_parse_context = parse_context<TK_ID>;

//the rule type for the parser
using parser_rule = rule<parser_parse_context>;

//parser ids
enum AST_ID {
	AST_NUM,
    AST_ADD,
    AST_SUB,
    AST_MUL,
    AST_DIV
};

//forward declaration
parser_rule expression;

//number
const auto number = terminal(TK_NUM)->*AST_NUM;

//value is either a number or a parenthesized expression
const auto value
    = number
    | TK_LPAR >> expression >> TK_RPAR
    ;
    
//multiplication/division has precedence over addition/subtraction
parser_rule mul 
    = (mul >> TK_MUL >> value)->*AST_MUL
    | (mul >> TK_DIV >> value)->*AST_DIV
    | value;
    
//addition/subtraction
parser_rule add 
    = (add >> TK_ADD >> mul)->*AST_ADD
    | (add >> TK_SUB >> mul)->*AST_SUB
    | mul;
    
//finally, the expression
parser_rule expression = add;

```

##### Using the parser

The results of the tokenizer can be fed to the parser by the parse context function `derive_parse_context`:

```cpp
parser_parse_context parser_pc = tokenizer_pc.derive_parse_context<AST_ID>();
expression.parse(parser_pc);
```

In this way, the output matches of the tokenizer are passed to the parser as input.

#### Tokenizing and parsing with one function call

The function `tokenize_and_parse(source, tokenizer_rule, parser_rule[, ast_factory])` can be used to do the tokenization and parsing with one call.

Example:

```cpp
const result = tokenize_and_parse(src, tokenizer, expression);
```

The result of the function above is a **shared pointer** to the following structure:

```cpp
template <class Source, class TokenizerParseContext, class ParserParseContext>
struct tokenize_and_parse_result {
    /** AST node type. */
    using ast_node_type = ast_node<typename ParserParseContext::source_type, typename ParserParseContext::match_id_type, typename ParserParseContext::text_position_type>;

    /** AST node pointer type. */
    using ast_node_ptr_type = std::shared_ptr<ast_node_type>;

    /** The tokenizer part. */
    struct {
        /** The tokenizer parse context. */
        TokenizerParseContext parse_context;

        /** The tokenizer parse result. */
        bool success;
    } tokenizer;

    /** The parser part. */
    struct {
        /** The parser parse context. */
        ParserParseContext parse_context;

        /** The parser parse result. */
        bool success;
    } parser;

    /** The created ast. */
    std::vector<ast_node_ptr_type> ast;

    /** tokenizer and parser success. */
    bool success;
    
    /** tokenizer and parser errors; exists only of tokenizer and parser share the same error type. */
    typename TokenizerParseContext::error_container_type errors;
};
```

The `tokenize_and_parse_result` srtucture contains all the information required for handling tokenization and parsing results.

#### Computing the result

Once all the above is in place, in order to calculate the result of an arithmetic expression, an evaluation function must be written.

That function will loop over an AST tree recursively, and calculate a number, depending on AST node type.

For example:

```cpp
template <class AST_PTR>
double eval(const AST_PTR& ast) {
	switch (ast->id()) {
    	case AST_NUM: {
        	std::stringstream stream;
            stream << ast->content();
            double v;
            stream >> v;
            return v;
        }
        
        case AST_ADD:
        	return eval(ast->children()[0]) + eval(ast->children()[1]);
        
        case AST_SUB:
        	return eval(ast->children()[0]) - eval(ast->children()[1]);
        
        case AST_MUL:
        	return eval(ast->children()[0]) * eval(ast->children()[1]);
        
        case AST_DIV:
        	return eval(ast->children()[0]) / eval(ast->children()[1]);
    }
    
    throw std::runtime_error("Invalid ast node type");
}
```

And finally, applying the `eval` function on the parse result:

```cpp
const double calculated_value = eval(result->ast[0]);
```

