# Parserlib

## Writing and using parsers

[Introduction](#introduction)

[Include files](#include-files)

[Namespace](#namespace)

[Declaring a `parser_engine` type](#declaring-a-`parser_engine`-type)

[Terminals](#terminals)

[Loops](#loops)

[Optionals](#optionals)

[Logical tests](#logical-tests)

[Sequences](#sequences)

[Branches](#branches)

[Matches](#matches)

[Rules](#rules)

[Using a parser](#using-a-parser)

[Abstract Syntax Tree nodes](#abstract-syntax-tree-nodes)

[Custom AST nodes](#custom-ast-nodes)

[Contextual tokenization](#contextual-tokenization)

[Contextual parsing](#contextual-parsing)

### Introduction

Writing a parser means to use the `class parser_engine<SourceT, MatchIdT>` to write grammars and parse some input based on those grammars.

The `class parser_engine<SourceT, MatchIdT>` provides classes and functions that are used to write grammars. It provides the following functionality:

- terminal parser classes:
	- atom
	- atom sequence
	- set
	- range
- non-terminal parser classes:
	- zero or more times loop
	- one or more times loop
	- optional
	- logical AND
	- logical NOT
	- sequence
	- choice
	- match
- `class rule` that can be used for recursive grammars.
- grammar operators: 
	- zero or more: unary `operator *`
	- one or more: unary `operator +`
	- optional: unary `operator -`
	- logical AND: `unary operator &`
	- logical NOT: `unary operator !`
	- sequence: binary `operator >>`
	- choice: binary `operator |`
	- exclusion: binary `operator -`
	- match: binary `operator ->*`

By combining the terminal and non terminal expressions, a grammar may be developed.

For example, parsing an IP4 address can be done with the following grammar:

```cpp
//a single digit
auto digit = range('0', '9');

//1, 2 or 3 digits
auto digits = digit >> -(digit >> -digit);

//4 series of digits, separated by '.'
auto ip4_address = digits >> '.' >> digits >> '.' >> digits >> '.' >> digits;
```

### Include files

In order to use the `class parser_engine<SourceT, MatchIdT>`, the following header must be included:

```cpp
#include "parserlib/parser_engine.hpp"
```

### Namespace

The `namespace parserlib` is the one that includes the `class parser_engine<SourceT, MatchIdT>`:

```cpp
using namespace parserlib;
```

### Declaring a `parser_engine` type

the `class parser_engine<SourceT, MatchIdT>` can be parameterized on:

- the source type: the parameter `SourceT` should be an STL-like container that contains the input. The parameter defaults to `std::string`.
- the match id type: the parameter `MatchIdT` is used for the type of the id stored in the parser matches and in AST (Abstract Syntax Tree) nodes. The parameter defaults to `int`.

For example, parsing a vector of odd/even integers can be done like this:

```cpp
enum class match_type {
    odd,
    even
};

using odd_even_parser_engine = parser_engine<std::vector<int>, match_type>;
```

### Terminals

The following types of terminals are provided by the `class parser_engine`:

- values
- sequence of values
- sets of values
- range of values

#### Terminal values

The function `parser_engine<SourceT, MatchIdT>::terminal` can be used to add a terminal value to a grammar. Examples:

```cpp
//parsing characters
using pe = parser_engine<>;
pe::terminal('a');

//parsing integers
using pe = parser_engine<std::vector<int>>;
pe::terminal(15); 

//parsing tokens
using pe = parser_engine<std::vector<token>>;
pe::terminal(token_type::identifier);
```

A terminal can also be parsed with one of the std functions like `std::isalpha`. For example:

```cpp
auto grammar = +terminal(&isalpha);
```

This functionality allows to write parsers for languages that accept identifiers that can accept a letter in any human language, e.g. Java.

#### Terminal sequences of values

The function `parser_engine<SourceT, MatchIdT>::terminal` can be used to add a sequence of terminal values to a grammar. Examples:

```cpp
//parsing character sequences
using pe = parser_engine<>;
pe::terminal("struct");

//parsing integer sequences
using pe = parser_engine<std::vector<int>>;
pe::terminal({15, 16, 17, 18}); 
```
#### Terminal sets

The functions `parser_engine<SourceT, MatchIdT>::one_of/set` can be used to add a set of terminal values to a grammar. 

Both functions are exactly the same, only the name differs. 

When a set parser is invoked to parse, it is successful if the current element to parse belongs to that set.

Examples:

```cpp
//parsing signs
using pe = parser_engine<>;
pe::one_of("+-");

//parsing integers
using pe = parser_engine<std::vector<int>>;
pe::one_of({15, 16, 17, 18}); 
```

#### Terminal ranges

The function `parser_engine<SourceT, MatchIdT>::range` can be used to add a range of terminal values to a grammar. 

When a range parser is invoked to parse, it is successful if the current element is within the given range.

Examples:

```cpp
using pe = parser_engine<>;

//parsing lower case letters
pe::range('a', 'z');

//parsing upper case letters
pe::range('A', 'Z');

//parsing digits
pe::range('0', '9');
```

### Loops

#### Zero or more times loop

The unary `operator *` can be used on an expression to create a loop that parses successfully zero or more times. Example:

```cpp
//parse 'a' zero or more times.
*terminal('a');
```

#### One or more times loop

The unary `operator +` can be used on an expression to create a loop that parses successfully one or more times. Example:

```cpp
//parse 'a' one or more times.
+terminal('a');
```

### Optionals

The unary `operator +` can be used on an expression to make it optional. Example:

```cpp
//optionally parse 'a'.
-terminal('a');
```

### Logical tests

Sometimes there is a need to parse an input only to see if it matches to a certain grammar, without producing any matches. For this case, there are two available options:

- logical AND
- logical NOT

#### Logical AND

The unary `operator &` can be used to test an expression. Example:

```cpp
//check if the grammar starts with 'a'
&terminal('a');
```

#### Logical NOT

The unary `operator !` can be used to test the negation of an expression. Example:

```cpp
//check if the grammar does not start with 'a'
!terminal('a');
```

#### Using tests for exclusions

Logical tests are useful for writing exclusion patterns. 

The binary `operator -` is used for 'subtracting' an expression from another expression, i.e. create an exclusion pattern.

Example:

```cpp
//parse any character except a double quote
range(0, 255) - terminal('"');
```

The operation above creates a sequence of the negation of the right argument of binary `operator -` with the left argument of the operator:

```cpp
!terminal('"') >> range(0, 255)
```

### Sequences

The binary `operator >>` can be used to form sequences.

If one member of a sequence fails, then the whole sequence fails.

When a sequence fails, the parser state is rewound to the state before the sequence, giving a chance to alternative grammar expressions to be parsed.

Examples:

```cpp
//identifier
auto identifier = letter >> *(letter | digit | '_');
```

### Branches

The binary `operator |` can be used to form a choice of different expressions (called branches).

If one branch fails, then the parser state is rewound to the state at the start of the choice, then the next branch is tried.

A choice parser is successful if at least one branch of it is succesful.

Examples:

```cpp
auto choices = terminal('a') | 'b' | 'c';
```

### Matches

In order to allow the parser engine to identify an input, a match should wrap a grammar expression.

The binary `operator ->*` is used to add a match to a grammar.

The left side of the operator should contain the grammar expression to parse.

The right side of the operator should contain the match id.

For example:

```cpp
enum class match_type {
    a, b, c
};

auto grammar = terminal('a') ->* match_type::a 
             | terminal('b') ->* match_type::b
             | terminal('c') ->* match_type::c;
```

In this way:

- when 'a' is parsed, the engine will store a match of type 'a' in the parse context.
- when 'b' is parsed, the engine will store a match of type 'b' in the parse context.
- when 'c' is parsed, the engine will store a match of type 'c' in the parse context.


### Rules

A rule is a grammar element that can be recursive. 

In order to support recursion, a variable of type `parser_engine<SourceT, MatchIdT>::rule` shall be declared, either as global/local variable or as a member of a class.

The rule variable must be constructed with/assigned from a grammar expression.

The `rule` class is also used to support left-recursive grammars.

Examples:

```cpp
//a rule of integer sequences
pe::rule integer_sequence = integer >> integer_sequence
                          | integer;

//definition of 'add' rule for a calculator grammar
pe::rule add = add >> '+' >> mul
             | add >> '-' >> mul
             | mul;
```

### Using a parser

The `class parser_engine<SourceT, MatchIdT>` contains the function `std::tuple<bool, ast_node_container_type, iterator_type> parse(SourceT& input, ParserT&& grammar [, const parse_options& options])` which can be used to parse an input.

Example:

```cpp
//for convenience
using pe = parser_engine<>;

//the grammar
auto grammar = terminal('a') >> 'b' >> 'c';

//the input
std::string input = "abc";

//parse some input
auto [success, ast, it] = pe::parse(input, grammar);
```

#### Parsing arguments

The `parse` function accepts the following arguments:

- `input`: reference to STL container that contains the source to be parsed; it must be in scope while the results of the `parse` function are in scope, because the function returns views (`std::basic_string_view`) on the source.
- `grammar`: the root expression of the grammar; it can be any grammar expression, or a rule.
- `parse_options`: optional structure of parse options; see below for usage.

#### Parsing result

The function `parse` returns a tuple of:

- a flag that says if parsing suceeded and the whole input is consumed, or there was an error.
- a vector of shared pointers to AST nodes; this contains what is matched by the match parsers of the grammar.
- an iterator of the source where parsing stopped; if parsing succeeds, it is equal to input.end().

### Abstract Syntax Tree nodes

One of the results of the `parse` function is an `std::vector<ast_node_ptr_type>`. It contains a list of ast nodes, as they are created from the matches of the parse context.

Each ast node has a match id, from the type given to the `class parser_engine`, parameter `MatchIdT`.

An AST node has the following interface:

- `MatchIDT get_id() const`: returns the id of the ast node.
- `const iterator_type& get_start_position() const`: returns the start position of the node inside the source.
- `const iterator_type& get_end_position() const`: returns the end position of the node inside the source.
- `std::basic_string_view<terminal_value_type> get_source() const`: returns a view on the source that corresponds to the parsed contents of the ast node.
- `const std::vector<sst_node_ptr_type>& get_children() const`: returns the children ast nodes.
- `ast_node_ptr_type get_parent() const`: returns the parent ast node.
- `void print(std::basic_ostream<Elem, Traits>& stream, size_t depth = 0, size_t tabSize = 4)`: prints the ast node and its children into the given output stream.

### Custom AST nodes

The `class parser_engine::ast_node` has a virtual destructor, and therefore it can be inherited.

In order to create custom ast nodes, the `class parser_engine::parse_options` contains a member `create_ast_node` which can be set to the function that creates an ast node. For example:

```cpp
class ast_node_a : public pe::ast_node {
public:
	ast_node_a(ast_id id, const iterator_type& start_position, const iterator_type& end_position, ast_node_container_type&& children) 
    : pe::ast_node(id, start_position, end_position, std::move(children)
    { 
    }
};

class ast_node_b : public pe::ast_node {
public:
	ast_node_b(ast_id id, const iterator_type& start_position, const iterator_type& end_position, ast_node_container_type&& children) 
    : pe::ast_node(id, start_position, end_position, std::move(children))
    { 
    }
};

class ast_node_c : public pe::ast_node {
public:
	ast_node_c(ast_id id, const iterator_type& start_position, const iterator_type& end_position, ast_node_container_type&& children) 
    : pe::ast_node(id, start_position, end_position, std::move(children))
    { 
    }
};

pe::ast_node_ptr_type my_create_ast_node(ast_id id, const iterator_type& start_position, const iterator_type& end_position, ast_node_container_type&& children) {
	switch (id) {
    	case ast_id::a:
        	return std::make_shared<ast_node_a>(id, start_position, end_position, std::move(children);
    	case ast_id::b:
        	return std::make_shared<ast_node_b>(id, start_position, end_position, std::move(children));
    	case ast_id::c:
        	return std::make_shared<ast_node_c>(id, start_position, end_position, std::move(children));
    }
    
    throw std::logic_error("invalid ast id");
}

parse_options options;
options.create_ast_node = &my_create_ast_node;
pe.parse(input, grammar, options);
```

### Using parse options to customize rule parsing

Sometimes the need arises to provide custom side effects from parsing some input; for example, handling multiple errors, or solving grammar ambiguities by looking up some other tables.

In order to provide custom parsing, and not litter the grammar with specific functionality, the parserlib library allows the definition of custom rule handler functions, as a member of the `class parse_options`.

Example:

```cpp
pe::parse_result my_custom_rule_handler(parse_context& pc, rule& r, parse_result result, void* custom_data) {
	return result;
}

pe::rule rule1 = terminal('a');
pe::rule rule2 = terminal('b');
auto grammar = rule1 | rule2;

pe::parse_options options;
options.rule_handlers[&rule1] = &my_custom_rule_handler;
options.rule_handlers[&rule2] = &my_custom_rule_handler;
options.custom_data = &my_custom_data;

std::string input = "a";
auto [success, ast, it] = pe::parse(input, grammar, options);
```

In the above grammar, when `rule1` or `rule2` are parsed, the function `my_custom_rule_handler` will be invoked, with the custom data specified in the options.

This allows to a) keep the grammar clean from side effects, b) allow side effects to happen when parsing a rule.

### Contextual tokenization

In some programming languages, the symbol `>>` will be parsed over `>`, due to the 'longest parsing rule'. However, in special sections of the grammar, for example in `c++`'s templates or `Java`'s generics, the symbol `>` will take precedence over the symbol `>>`.

This can be expressed very naturally with `parserlib`, by exploiting the fact that parsers are called in the order they are declared, and therefore it is possible to declare a whole sequence of tokens that takes precedence over the rest of the tokens.

Here is an example (taken from the unit tests):

```cpp
    enum token_type {
        IDENTIFIER,
        LEFT_SHIFT,
        RIGHT_SHIFT,
        LESS_THAN,
        GREATER_THAN,
    };

    auto whitespace = terminal(' ');

    auto letter = pe::range('a', 'z') | pe::range('A', 'Z');
    auto digit = pe::range('0', '9');
    auto identifier = (letter >> *(letter | digit)) ->* IDENTIFIER;

    auto left_shift = pe::terminal("<<") ->* LEFT_SHIFT;
    auto right_shift = pe::terminal(">>") ->* RIGHT_SHIFT;

    auto less_than = pe::terminal('<') ->* LESS_THAN;
    auto greater_than = pe::terminal('>') ->* GREATER_THAN;

    pe::rule generics_specification
        = identifier >> less_than >> -generics_specification >> greater_than
        | identifier;

    const auto token
        = whitespace
        | generics_specification
        | identifier
        | left_shift
        | right_shift
        | less_than
        | greater_than;

    auto grammar = *token;
```

When using the above grammar, we can do the following parsing:

```cpp
	string input = ">>><<<foo<bar<cee>>><<>><";
```

The series of produced tokens is the following:

```cpp
    RIGHT_SHIFT
    GREATER_THAN
    LEFT_SHIFT
    LESS_THAN
    IDENTIFIER
    LESS_THAN
    IDENTIFIER
    LESS_THAN
    IDENTIFIER
    GREATER_THAN
    GREATER_THAN
    GREATER_THAN
    LEFT_SHIFT
    RIGHT_SHIFT
    LESS_THAN
```

Which means the symbol `>` was parsed with increased precedence over the symbol `>>` within the context of a `generics specification` rule.

### Contextual parsing

Sometimes, some part of a grammar can be parsed in more than one ways. For example, in `c++`, the following code is ambiguous, as it can be either a pointer declaration or a multiplication:

```ppp
//ambiguous in c++; it might be one of the following:
//1) declaration of variable y of type x*.
//2) multiplication of variable x to variable y.
x*y;
```

#### Solving ambiguities by using custom match functions

In order to resolve the ambiguity, `parserlib` allows the user to provide a custom match function, which allows lookup in previous matches. For example:

```cpp
	auto resolve_ambiguous_grammar = [](parse_context& pc, match_container_type& matches) {
    	...
    };

	auto ambiguous_grammar = (x >> y) ->* resolve_ambiguous_grammar;
```

When a custom match function is used, it is given two parameters:

- the current parse context, which contains the matches found so far. They can be used to discover previously parsed symbols.
- an array of children matches, which can be used to derive the appropriate match id from the children; this can be modified, so as that the produced match has a different set of children from the one parsed.

Here is a complete example of a custom match function, taken from the unit tests.

It is rather long, because it defines a tokenizer and parser, but it deserves to be studied as it provides a full example of the library's capabilities:

```cpp
static void test_contextual_parsing() {
    /* tokenizer */

    enum TOKEN {
        IDENTIFIER,
        INTEGER,
        STAR,
        ASSIGN,
        SEMICOLON,
        TYPEDEF,
        INT
    };

    auto whitespace = range('\0', ' ');

    auto digit = range('0', '9');
    auto letter = range('a', 'z') | range('A', 'Z');

    auto typedef_ = terminal("typedef") ->* TYPEDEF;

    auto int_ = terminal("int") ->* INT;

    auto identifier = (letter >> *(letter | digit | '_')) ->* IDENTIFIER;

    auto integer   = +digit              ->* INTEGER  ;
    auto star      = terminal('*')       ->* STAR     ;
    auto assign    = terminal('=')       ->* ASSIGN   ;
    auto semicolon = terminal(';')       ->* SEMICOLON;

    auto token
        = whitespace
        | typedef_
        | int_
        | identifier
        | integer
        | star
        | assign
        | semicolon;

    auto tokenizer_grammar = *token;

    /* parser */

    using parser_pe = parser_engine<ast_node_container_type>;

    enum AST {
        TYPE_INT,
        TYPE_NAME,
        TYPE_POINTER,
        DECLARATION_TYPEDEF,
        DECLARATION_VARIABLE,
        EXPRESSION_MULTIPLICATION,
        EXPRESSION_NAME,
        EXPRESSION_INTEGER,
        VAR_NAME
    };

    auto base_type
        = parser_pe::terminal(INT) ->* TYPE_INT
        | parser_pe::terminal(IDENTIFIER) ->* TYPE_NAME;

    auto pointer_type
        = (base_type >> STAR) ->* TYPE_POINTER
        | base_type;

    auto type_expression = pointer_type;

    auto typedef_declaration = (parser_pe::terminal(TYPEDEF) >> type_expression >> IDENTIFIER >> SEMICOLON) ->* DECLARATION_TYPEDEF;

    auto value
        = parser_pe::terminal(INTEGER) ->* EXPRESSION_INTEGER
        | parser_pe::terminal(IDENTIFIER) ->* EXPRESSION_NAME;

    auto multiplication
        = (value >> STAR >> value) ->* EXPRESSION_MULTIPLICATION
        | value;

    auto expression = multiplication;

    auto match_variable_or_multiplication = [](parser_pe::parse_context& pc, parser_pe::match_container_type& matches) {
        //in order for the variable declaration to look like a multiplication,
        //it shall have two members
        if (matches.size() != 2) {
            return DECLARATION_VARIABLE;
        }

        const auto pointerTypeMatch = matches[0];

        //if the first member is not a pointer type, 
        //then the declaration is a variable
        if (pointerTypeMatch.get_id() != TYPE_POINTER) {
            return DECLARATION_VARIABLE;
        }

        const auto typenameMatch = pointerTypeMatch[0];
           
        //if the base type of the pointer type is not a type name,
        //then the declaration is a variable
        if (typenameMatch.get_id() != TYPE_NAME) {
            return DECLARATION_VARIABLE;
        }

        //the type name that might be a variable identifier 
        const auto id = (*typenameMatch.get_start_position())->get_source();

        //for a multiplication to be valid, there must be a previous variable declaration that has an identifier
        //equal to the id found above
        for (const auto& m : pc.get_matches()) {
            if (m.get_id() == DECLARATION_VARIABLE) {
                auto varName = (*m.find_child_by_id(VAR_NAME)->get_start_position())->get_source();
                if (varName == id) {
                    //replace the pointer match with an expression name match
                    matches[0] = parser_pe::match(EXPRESSION_NAME, typenameMatch.get_start_position(), typenameMatch.get_end_position());
                    return EXPRESSION_MULTIPLICATION;
                }
            }
        }

        //did not find a variable with the given name; make the match a variable
        return DECLARATION_VARIABLE;
        };

    auto variable_declaration = (type_expression >> (parser_pe::terminal(IDENTIFIER) ->* VAR_NAME) >> -(ASSIGN >> expression) >> SEMICOLON) ->* match_variable_or_multiplication;

    auto declaration
        = typedef_declaration
        | variable_declaration;

    auto parser_grammar = *declaration;

    {
        string input = 
            "typedef int x;"
            "int y = 0;"
            "x* a;"
            "y* b;"
            ;

        auto [tokenizer_success, tokens, token_it] = pe::parse(input, tokenizer_grammar);

        auto [parser_success, ast, parser_it] = parser_pe::parse(tokens, parser_grammar);

        assert(ast.size() == 4);
        assert(ast[0]->get_id() == DECLARATION_TYPEDEF);
        assert(ast[1]->get_id() == DECLARATION_VARIABLE);
        assert(ast[2]->get_id() == DECLARATION_VARIABLE);
        assert(ast[3]->get_id() == EXPRESSION_MULTIPLICATION);
    }
}
```

In the above example, the code `x* a` is parsed as a variable declaration, because there is no `x` variable previously declared, whereas the code `y* a` is parsed as a multiplication, because there is a `y` variable already declared.

#### Solving ambiguities by using custom AST creation functions

When invoking the `parse` function of a `parser_engine` type instance, one of the options paased is a custom ast node creation function.

An ast node creation function has the following type signature:

```cpp
ast_node_ptr_type(match_id_type id, const iterator_type& start_position, const iterator_type& end_position, ast_node_container_type&& children);
```

A custom ast node creation function could examine the children in order to create an AST node with a different match id from the one passed to the function, and with a different set of children.

For how to use custom ast node creation functions, see the section [Custom AST nodes](#custom-ast-nodes).
