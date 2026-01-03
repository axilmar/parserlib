# Parserlib

## Writing a parser

* [Namespace and include file](#namespace-and-include-file)
* [What is a parser](#what-is-a-parser)
* [Functions](#functions)
* [Operators](#operators)
* [Matches](#matches)
* [Rules](#rules)
* [Errors](#errors)
* [A calculator example](#a-calculator-example)


### Namespace and include file

The file `parserlib.hpp` is the master header.

Every symbol is included in namespace `parserlib`.

```cpp
#include "parserlib.hpp"
using namespace parserlib;
```

### What is a parser

A parser is a tree of objects, called `parse nodes`, which are created by calling functions and applying operators on the results of those functions or operators.

The functions are used to create the primitive parse node types (terminals, sets, ranges, etc).

The operators are used to create loops, make parse node optionals, create sequences or choices, etc. The operators make the grammar easy to read, because they resemble ENBF operators.

Each parse node implements a parse function which accepts a `parse context`, which contains the parser state and allows manipulation of it via a specific API.

The library can parse anything, from any container, and from any file, and can be used to implement a simple character parser, or a lexer-parser combination.

### Functions

THe function `terminal` is used to create parse nodes for single symbols, or strings, or other symbol values (for example, enumerations).

The function `set` is used to create a parse node that parses a symbol out of a set.

The function `range` is used to create a parse node that parses a symbol from within a range of symbols.

The function `any` is used to parse any single symbol.

The function `end` can be used to test if the input has ended (in order to make it an error to only parse an input partially).

The function `newline` can be used on character parsers to increment the line counter, allowing column/line information on matches.

The function `function` can be used to create parse nodes out of lambda functions and out of pointers to functions.

Examples:

```cpp
auto star = terminal('*');
auto struct_keyword = terminal("struct");
auto digit = range('0', '9');
auto hex_digit = set("0123456789abcdefABCDEF");
auto nl = newline('\n');
```
### Operators

#### Unary operators

The `unary opeator *` can be used to create a loop that invokes another parse node 0 or more times.

The `unary opeator +` can be used to create a loop that invokes another parse node 1 or more times.

The `unary opeator -` can be used to make another parse node optional.

The `unary operator &` can be used to create a positive logical test out of a parse node.

The `unary operator -` can be used to create a negative logical test out of a parse node.

Examples:

```cpp
auto parse_0_or_more_a = *terminal('a');
auto parse_1_or_more_b = +terminal('b');
auto parse_c_optionally = -terminal('c');
auto test_for_a = &terminal('a');
auto test_for_not_a = !terminal('a');
```

#### Binary operators

The `bnary opeator >>` can be used to create a sequence of parse nodes.

The `bnary opeator -` can be used to create an exclusion of a parse node from another parse node.

The `bnary opeator |` can be used to create a choice of parse nodes.

The `bnary opeator ->*` can be used to create a match out of a parse node.

Examples:

```cpp
auto class_member = constructor | destructor | getter | setter | method | type_decl;
auto class_def = terminal("class") >> '{' >> *class_member >> '}';
auto comment_char = any - "*)";
auto match_class = class_def->*MATCH_ID::CLASS_DEF;
```

### Matches

During parsing, the current `parse context` contains a container of matches.

The match `operator ->*` is used to create a match parse node, i.e. a parse node that, when another parse node parses successfully, the match parse node adds a match to the parse context.

Matches can have children: a match that is created within another match, becomes child of that match.

Matches then form a tree.

A match consists of a match id, i.e. a value which identifies that particular match, an iterator range, and a container of children.

Usually, the match id is an enumeration.

Example:

```cpp
enum class PARSER_MATCH_ID {
	FUNCTION_DEF,
    CLASS_DEF,
    TYPE_DEF,
    TYPE_PTR,
    TYPE_FUNC,
    YUPE_INT,
    BLOCK_STM,
    IF_STM,
    IF_EXPR,
    ADD_EXPR,
    MUL_EXPR,
    ...
};

auto function_def = (terminal(LEXER_MATCH_ID::FUNCTION) >> LEXER_MATCH_ID::IDENTIFIER >> LEXER_MATCH_ID::LEFT_PAREN >> arguments >> LEXER_MATCH_ID::RIGHT_PAREN >> function_body)->*PARSER_MATCH_ID::FUNCTION_DEF;
```

Later, parse matches can be retrieved by the member function `get_matches()` on the parse context (more on that on the next section).

### Rules

Rules are special classes that allow recursive and left-recursive grammars.

They are templates which can be customized on the parse context type they will parse. This is necessary because in order to implement recursion, type erasure must be used (i.e. expressions wrapped into a generic parse node wrapper).

Rules employ some special algorithms that allow parsing of left recursive grammars, while also maintaining left-associativity.

Here are some examples:

```cpp
//using the default parse context type
using parse_context_type = parse_context<>;

//right-recursive grammar
rule<parse_context_type> list_of_a = 'a' >> list_of_a | 'a';

//left-recursive grammar
rule<parse_context_type> list_of_a = list_of_a >> 'a' | 'a';

```

### Errors

In some cases, it is desirable to continue parsing even if an error has happened.

The special function `error` can be used to introduce an error handler, which puts an error into a parse context's error container.

The function `error` is accompanied by two other special functions:

1. function `skip_before`: can be used to skip input to the point where a parse node started parsing.
2. function `skip_after`: can be used to skip input to the point where a parse node finished parsing.

Here are some examples:

```cpp
//if unknown text is found, skip it until the next valid token (or the end of input).
auto lexer = *(token | error(ERROR_ID::SYNTAX_ERROR, skip_before(token)));

//if there is an error in the statement, skip after the semicolon to the next statement or block end
auto stm_parser = *(stm | error(ERROR_ID::STM_ERROR, skip_after(';')));
```

When an error parse node is called to parse, it uses another node to find the end of error, then it adds an error instance in the parse context.

An error instance of composed of an id, and an iterator range.

The function `get_errors()` can be used on a parse context to process the errors, after parsing.


### A calculator example

Here is a calculator example that can be used to compute arithmetic expressions (without whitespace, for simplicity):

```cpp
enum {NUM, ADD, SUB, MUL, DIV};

using rule_type = rule<>;

rule_type add, mul;

auto digit = range('0', '9');

auto num = (+digit >> -('.' >> +digit))->*NUM;

auto val = '(' >> add >> ')'
         | num;

add = (add >> '+' >> mul)->*ADD
    | (add >> '-' >> mul)->*SUB
    | mul;

mul = (mul >> '*' >> val)->*MUL
    | (mul >> '/' >> val)->*DIV
    | val;

auto grammar = add;
```

