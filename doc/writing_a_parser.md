# Parserlib

## Writing a parser

* [How to write a parser](#how-to-write-a-parser)
	* [Functions](#functions)
		* [terminal](#function-'terminal')
		* [case_insensitive_terminal](#function-'case_insensitive_terminal')
		* [set](#function-'set')
		* [range](#function-'range')
		* [any](#function-'any')
		* [end](#function-'end')
		* [function](#function-'function')
		* [newline](#function-'newline')
		* [skip_before](#function-'skip_before')
		* [skip_after](#function-'skip_after')
		* [error](#function-'error')
		* [on_error](#function-'on_error')
		* [loop_break](#function-'loop_break')
		* [memoized](#function-'memoized')
	* [Rules](#rules)
	* [Operators](#operators)
		* [unary operator *](#unary-operator-'*')
		* [unary operator +](#unary-operator-'+')
		* [unary operator -](#unary-operator-'-')
		* [unary operator &](#unary-operator-'&')
		* [unary operator !](#unary-operator-'!')
		* [binary operator >>](#binary-operator-'>>')
		* [binary operator |](#binary-operator-'|')
		* [binary operator -](#binary-operator-'-')
		* [binary operator ->*](#binary-operator-'->*')
		* [binary operator >>=](#binary-operator-'>>=')
	* [Custom parser classes](#custom-parser-classes)
	* [Conversion of types to parse nodes](#conversion-of-types-to-parse-nodes)

### How to write a parser

A parser is a tree of parse nodes.

Each parse node in the tree uses and manipulates a parse context, which contains the state of parsing.

In order to build such a parse tree, the user might use:

- functions on data that create parse nodes.
- rules.
- operators on parse nodes or rules.

#### Functions

##### Function 'terminal'

The function `terminal` can be used to create symbol and symbol string parse nodes. The following overloads exist:

* `terminal(symbol)`: creates a parse node for the given symbol.
* `terminal(string)`: creates a parse node for the given string (null-terminated string or `std::basic_string`).
* `terminal(string, comparator)`: creates a parse node for the given string (null-terminated string or `std::basic_string`) with the specific comparator.

Examples:

```cpp
terminal('a');
terminal("hello world");
terminal(L"hello world");
terminal(50);
terminal(TOKEN_ID::INTEGER);
terminal("xml", case_insensitive_symbol_comparator());
```

##### Function 'case_insensitive_terminal'

The function `case_insensitive_terminal` can be used to create symbol and symbol string parse nodes that parse a symbol or string using the `case_insensitive_symbol_comparator` comparator.

Examples:

```cpp
case_insensitive_terminal('a');
case_insensitive_terminal("hello world");
case_insensitive_terminal(L"hello world");
```

##### Function 'set'

The function `set` can be used to create a parse node for a symbol that belongs in a set. 

Examples:

```cpp
set("0123456789ABCDEFabcdef");
set("+-");
set('0', '1', '2', '3');
```

##### Function 'range'

The function `range` can be used to create a parse node for a symbol within a specific range.

Examples:

```cpp
range('0', '9');
range('a', 'z');
range('A', 'Z');
```

##### Function 'any'

The function `any` can be used to create a parse node that parses any single symbol.

Example:
```cpp
any();
```

##### Function 'end'

The function `end` can be used to create a parse node that recognizes the end of input.

Example:
```cpp
end();
```

##### Function 'function'

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

##### Function 'newline'

The function `newline` can be used to create a parse node that increments a parse context's current line, when another parse node parses the given input successfully.

Example:

```cpp
newline('\n');
```

##### Function 'skip_before'

The function `skip_before(expression)` can be used to create a repeating loop of the given expression.

The loop stops when the given expression parses successfully.

This allows an error parser to skip input until the parsing state is not erroneous.

The state of parsing is set to the state just before the given expression parses successfully.

Example:

```cpp
error(SYNTAX_ERROR, skip_before(token));
```

##### Function 'skip_after'

The function `skip_after(expression)` can be used to create a repeating loop of the given expression.

The loop stops when the given expression parses successfully.

This allows an error parser to skip input until the parsing state is not erroneous.

The state of parsing is set to the state just after the given expression parses successfully.

Example:

```cpp
error(SYNTAX_ERROR, skip_after(TOKEN_SEMICOLON));
```

##### Function 'error'

The function `error(id, [skip_parser])` can be used to add an error to a parse context's error stack, and then allow parsing to continue from a specific point in the input.

The function `error` has the following signature:

The `skip_parse_node` parameter represents a parse node (or a value convertible to a parse node) that is used to identify where the error stops in the input; it is optional.

IF no skip parser is provided, then the parse position is not advanced.

This feature allows parsers to handle errors, then continue parsing.

Any parse node can be a skip parse node; side effects of parse nodes are canceled if used as skip parse nodes; the current parse context is not affected by them.

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
    | error(ERROR_ID::INVALID_CHARACTERS, skip_before(symbol));

const tokenizer = *token;
```

Error parse nodes return true.

If continuing parsing after an error is not desired, then the error declaration shall be followed by `false`.

Example:

```cpp
error(ERROR_UNEXPECTED) >> false;
```

#### Function 'on_error'

The function `on_error(parse_node, skip_parse_node)` allows continuation of parsing after the last error, to a point where the skip parse error defines.

It allows jumping to a forward parse position after an error happens within another parse node.

Example:

```cpp
const auto a = terminal('a');
const auto grammar = *on_error(a | error(1), skip_before(a));
```

#### Function 'loop_break'

The function `loop_break([levels = 1])` can be used to break one or more loops (depending on the `levels` parameter).

It is useful when an error is detected within a loop.

It works by throwing a `loop_break_exception`, which is cought by the loop parsers and handled accordingly.

Example:

```cpp
//loop over 'a'; if 'b' is met, stop the loop.
const auto loop = *((&terminal('b') >> error(ERROR_ID::INVALID_CHARACTER) >> loop_break()) | 'a');
```

#### Function 'memoized'

The function `memoized(<parse node>)` can be used to add memoization of the result of parsing a specific parse node.

It is useful when a parse node must be parsed repeatedly at a specific position in the source: instead of calling the parser, its parse results are recalled from internal parse context memory.

Example:

```cpp
memoized(f);
```

**Note**: *Memoizing a parser may result in slower parsing, due to memory management; it is advised that parsing durations are measured with and without memoization.*

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

The unary `operator *`, when applied on a parse node, puts the parse node in a loop that allows for an input to be repeated zero or more times.

Example:

```cpp
*terminal('a');
```

##### Unary operator '+'

The unary `operator +`, when applied on a parse node, puts the parse node in a loop that allows for an input to be repeated one or more times.

Example:

```cpp
+terminal('a');
```

##### Unary operator '-'

The unary `operator -`, when applied on a parse node, makes a parse node optional, i.e. if the parse node fails to parse then parsing continues as if the parse node suceeded.

Example:

```cpp
-terminal('a');
```

##### Unary operator '&'

The unary `operator &`, when applied on a parse node, makes a `logical and` parse node, i.e. a parse node that tests positively for a specific input.

A `logical and` parse node does not consume any input: the state of a parse context after the node returns is always the same as the state of the parse context before the parse node parses anything.

Example:

```cpp
&terminal('a');
```

##### Unary operator '!'

The unary `operator !`, when applied on a parse node, makes a `logical not` parse node, i.e. a parse node that tests negatively for a specific input.

A `logical not` parse node does not consume any input: the state of a parse context after the node returns is always the same as the state of the parse context before the parse node parses anything.

Example:

```cpp
!terminal('a');
```

##### Binary operator '>>'

The binary `operator >>` creates a sequence of parse nodes.

If a parse node in a sequence fails to parse, then the whole sequence fails, and the state of the parse context is restored to the state before the sequence is entered.

Example:

```cpp
terminal('a') >> 'b' >> 'c';
```

##### Binary operator '|'

The binary `operator |` creates a choice of parse nodes.

If a parse node in a choice fails to parse, then the next parse node of the choice is selected to parse, until all parse nods of a choice have been used.

After a parse node of a choices fails to parse, the state of the parse context is restored to the state of the parse context before the failed parse node was entered.

If no member of a choice could parse successfully, then a choice fails to parse.

Example:

```cpp
terminal('a') | 'b' | 'c';
```

##### Binary operator '-'

The binary `operator -` creates an exclusion pattern.

An expression of the form `A - B` is converted to `!B >> A`, allowing A to be entered only if B fails to parse.

Example:

```cpp
//all characters except the cell character
any() - '#';
```

##### Binary operator '->*'

The binary `operator ->*` creates a match parse node.

A match is an identified portion of the input with a specific match id value.

A parse context contains a stack of matches.

A match parse node places a match in a parse context's match stack, if another parse node parses successfully.

The expression is of the form `<parse_node>->*<match_id>`, where `match_id` refers to the identifier of the relevant portion.

Example:

```cpp
//when an identifier is recognized, push the relevant match to the match stack
const auto identifier = (letter >> *(letter | digit))->*TOKEN_ID::IDENTIFIER;
```

##### Binary operator '>>='

The binary `operator >>=` can be used to add a name to an expression.

Example:

```cpp
const auto integer = "integer" >>= +digit;
```

#### Custom parser classes

A parser class must have the following interface:

```cpp
class my_parser_class : public parserlib::parse_node<my_parser_class> {
public:
	template <class ParseContext> bool parse(ParseContext& pc) const;
};
```

Inheritance from `parserlib::parse_node` is desirable, in order to allow a parser class to get the unary operators that allow it to be converted to loops, optionals, etc.

If a parser class does not want to inherit `parse_node`, then it may inherit `parse_node_base`, in order to be part of sequences and choices.

#### Conversion of types to parse nodes

The function `make_parse_node` can be used to create a converter of a type to a parse node.

This allows a type to participate in parserlib DSL.

For example:

```cpp
struct MyCustomType;

class MyCustomTypeParser : public parse_node<MyCustomTypeParser> {
public:
	MyCustomTypeParser(MyCustomType v);
    template <class ParseContext>bool parse(ParseContext& pc) const;
};

MyCustomTypeParse make_parse_node(MyCustomType v) { return v; }

const auto grammar = terminal('a') >> MyCustomType('a') >> MyCustomType('b');
```

The following table summarizes which types can be converted to parse nodes via `make_parse_node`:

| Type                                    | parse node class                                          |
|-----------------------------------------|-----------------------------------------------------------|
| `< char type >`                         | `symbol_parse_node< char type >`                          |
| `const < char type > *`                 | `string_parse_node< char type >`                          |
| `std::basic_string< char type >`        | `string_parse_node< char type >`                          |
| `bool`                                  | `bool_parse_node`                                         |
| `bool (*)(< parse context type >)`      | `function_parse_node< bool (*)(< parse context type >) >` |
| `bool [] (< parse context type >) `     | `function_parse_node< bool [](< parse context type >) >`  |
| `rule< parse context type >`            | `rule_ref_parse_node < parse context type >`              |

Char types are:

* `signed char`
* `unsigned char`
* `char`
* `wchar_t`
* `char8_t` (only if it is defined; by default, it is not defined in `c++17`)
* `char16_t`
* `char32_t`
