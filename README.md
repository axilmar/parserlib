# Parserlib

**Table Of Contents**

[TOC]

## Introduction

`Parserlib` is a `c++17` header only library that allows building of recursive-descent parsers using EBNF-like syntax.

**Features**

* c++17 (tested with msvc and gcc).
* header-only.
* recursive-descent parsing.
* parsing of left-recursive grammars.
* EBNF-like syntax.
* character/custom type parsing.
* extensible via templates.
* multiple error handling.
* ASTs (Abstract Syntax Trees).

**Version**

1.0.0.9

**Quick example**

The following program implements and uses a calculator parser on strings:

```cpp
#include "parserlib.hpp"
using namespace parserlib;

extern rule<> expr;

const auto digit = range('0', '9');

const auto number = +digit >> -('.' >> +digit);

rule<> val = number
           | '(' >> expr >> ')';

rule<> mul = mul >> '*' >> val
           | mul >> '/' >> val
           | val;

rule<> add = add >> '+' >> mul
           | add >> '-' >> mul
           | mul;

rule<> expr = add;

int main() {
	std::string input = "1+2/(3*4)";
    parse_context<> pc(input);
    const bool result = expr.parse(pc);
    return 0;
}
```

## Documentation

### Using the library

`Parserlib` is a heavily templated `c++` library, and therefore it lives only as a set of headers, which are located in the `include` folder.

In order to be used, these headers should be put somewhere in a project's include path.

The main header, that includes everything, is `parserlib.hpp`.

The namespace is `parserlib`.

The following code includes the main header and uses the library's namespace:

```cpp
#include "parserlib.hpp"
using namespace parserlib;
```

### Writing a parser

A parser is a tree of parse nodes.

Each parse node in the tree uses and manipulates a parse context, which contains the state of parsing.

In order to build such a parse tree, the user might use:

- functions on data that create parse nodes.
- rules.
- operators on parse nodes or rules.

#### Functions

##### Function "terminal"

The function `terminal` can be used to create symbol and symbol string parse nodes.

Examples:

```cpp
terminal('a');
terminal("hello world");
terminal(L"hello world");
terminal(50);
terminal(TOKEN_ID::INTEGER);
```

##### Function "set"

The function `set` can be used to create a parse node for a symbol that belongs in a set. 

Examples:

```cpp
set("0123456789ABCDEFabcdef");
set("+-");
```

##### Function "range"

The function `range` can be used to create a parse node for a symbol within a specific range.

Examples:

```cpp
range('0', '9');
range('a', 'z');
range('A', 'Z');
```

##### Function "any"

The function `any` can be used to create a parse node that parses any single symbol.

Example:
```cpp
any();
```

##### Function "end"

The function `end` can be used to create a parse node that recognizes the end of input.

Example:
```cpp
end();
```

##### Function "function"

The function `function` can be used to create a parse node for a function, a functor or a lambda.

If a function/functor/lambda is part of a binary expression, in which at least one of the operands is a parse node, then there is no need to use the function `function`.

**NOTE**: *Functors and lambdas can have the parse context parameter templated, whereas a simple function cannot.*

Example:
```cpp
bool function1(parse_context<>& pc) {
	...;
	return true;
}

class Functor1 {
public:
	template <class ParseContext>
	bool operator ()(ParseContext& pc) {
    	...;
        return true;
    }
};
Functor1 functor1;

function(function1);
function(functor1);
function([](auto& pc){ ...; return true; });
```

##### Function "newline"

The function `newline` can be used to create a parse node that increments a parse context's current line, when another parse node parses the given input successfully.

Example:

```cpp
newline('\n');
```

##### Function "error"

The function `error` can be used to add an error to a parse context's error stack, and then allow parsing to continue from a specific point in the input.

The function `error` has the following signature:

```cpp
template <class ErrorId, class SkipParseNode> auto error(const ErrorId& id, SkipParseNode&& skip_parse_node);
```

The `skip_parse_node` parameter represents a parse node (or a value convertible to a parse node) that is used to identify where the error stops in the input.

The error parse node loops over the input, using the skip parse node, until the skip parse node parses succcesfully, then an error is added to the current parse context.

This feature allows parsers to handle errors, then continue parsing.

Any parse node can be a skip parse node; side effects of parse nodes are cancelled if used as skip parse nodes; the current parse context is not affected by them.

For example:
```cpp
const auto symbol
	= terminal(' ')
    | newline('\n')
    | number
    | plus
    | minus
    | star
    | div
    | left_parenthesis
    | right_parenthesis;

//if a valid symbol is not recognized, add an error and proceed to the next symbol
const auto token
    = symbol
    | error(ERROR_ID::INVALID_CHARACTERS, symbol);

const tokenizer = *token;
```

#### Rules

Rules are special parse node objects that are used for recursive parser declarations.

In c++, an object can be used by reference if its definition is complete at the point of a variable declaration, even if the object is not constructed yet; this allows recursive parser declarations by keeping references to `rule` instances.

Rules are also managed by reference when copy-assigned or copy-constructed.

Rules internally use a polymorphic parser wrapper class, in order to be allowed to be used before constructed, and therefore rules are the only objects that are parameterized by the parse context they expect to handle.

The `rule` class has the following signature:

```cpp
template <clas ParseContext = parse_context<>> class rule;
```

Examples:

```cpp
//a non-left recursive rule
rule<> a = 'b' >> a
         | 'a';

//a left recursive rule
rule<> a = a >> 'b'
         | 'a';
         
//it does not copy rule 'a', it refers to rule 'a' via a rule reference instance         
rule<> e = a;          
```

Rules can handle parsing left-recursive grammars.

#### Operators

##### Unary operator '*'

The unary operator '*', when applied on a parse node, puts the parse node in a loop that allows for an input to be repeated zero or more times.

Example:

```cpp
*terminal('a');
```

##### Unary operator '+'

The unary operator '+', when applied on a parse node, puts the parse node in a loop that allows for an input to be repeated one or more times.

Example:

```cpp
+terminal('a');
```

##### Unary operator '-'

The unary operator '-', when applied on a parse node, makes a parse node optional, i.e. if the parse node fails to parse then parsing continues as if the parse node suceeded.

Example:

```cpp
-terminal('a');
```

##### Unary operator '&'

The unary operator '&', when applied on a parse node, makes a `logical and` parse node, i.e. a parse node that tests positively for a specific input.

A `logical and` parse node does not consume any input: the state of a parse context after the node returns is always the same as the state of the parse context before the parse node parses anything.

Example:

```cpp
&terminal('a');
```

##### Unary operator '!'

The unary operator '!', when applied on a parse node, makes a `logical not` parse node, i.e. a parse node that tests negatively for a specific input.

A `logical not` parse node does not consume any input: the state of a parse context after the node returns is always the same as the state of the parse context before the parse node parses anything.

Example:

```cpp
!terminal('a');
```

##### Binary operator '>>'

The binary operator '>>' creates a sequence of parse nodes.

If a parse node in a sequence fails to parse, then the whole sequence fails, and the state of the parse context is restored to the state before the sequence is entered.

Example:

```cpp
terminal('a') >> 'b' >> 'c';
```

##### Binary operator '|'

The binary operator '|' creates a choice of parse nodes.

If a parse node in a choice fails to parse, then the next parse node of the choice is selected to parse, until all parse nods of a choice have been used.

After a parse node of a choices fails to parse, the state of the parse context is restored to the state of the parse context before the failed parse node was entered.

If no member of a choice could parse successfully, then a choice fails to parse.

Example:

```cpp
terminal('a') | 'b' | 'c';
```

##### Binary operator '-'

The binary operator '-' creates an exclusion pattern.

An expression of the form `A - B` is converted to `!B >> A`, allowing A to be entered only if B fails to parse.

Example:

```cpp
//all characters except the cell character
any() - '#';
```

##### Binary operator '->*'

The binary operator '->*' creates a match parse node.

A match is an identified portion of the input with a specific match id value.

A parse context contains a stack of matches.

A match parse node places a match in a parse context's match stack, if another parse node parses successfully.

The expression is of the form `<parse_node>->*<match_id>`, where `match_id` refers to the identifier of the relevant portion.

Example:

```cpp
//when an identifier is recognized, push the relevant match to the match stack
const auto identifier = (letter >> *(letter | digit))->*TOKEN_ID::IDENTIFIER;
```

### Using a parser

In order to use a parser, a parse context must be initialized from an input (or an input range), then passed to the parser.

Examples:

```cpp
std::string input = "123";
parse_context pc{ input };
//alternatively
//parse_context pc{ input.begin(), input.end() };
const bool result = parser.parse(pc);
```

#### The parse_context class

The class `parse_context` plays the most important role in this library, as it is used to keep the parsing state, which includes:

* current parse position
* match stack
* error stack
* rule state (used in left recursion)

The class `parse_context` provides an API for manipulating and testing the above state, and that API is used by the parse nodes.

##### Customizing a parse context

The `parse_context` class has the following signature:

```cpp
template <
    class Source = default_source_type,
    class MatchId = default_match_id_type,
    class ErrorId = default_error_id_type,
    class TextPosition = default_text_position_type,
    class SymbolComparator = default_symbol_comparator_type,
    class... Extensions
>
class parse_context : public Extensions...;
```

###### Parameter 'Source'

The `Source` parameter of a `parse_context` class represents the type of input to parse.

It can be any STL-like container.

Example:

```cpp
std::vector<point> points;
parse_context pc{ points; };
```
