# Parserlib

## Writing parsers

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

The rule variable must be constructed/assigned a grammar expression.

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
