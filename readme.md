# Parserlib

A `c++17` recursive-descent parser library that:

- provides an easy EBNF-like C++ domain-specific-language (DSL) for writing grammars.
- allows any type of STL-like container or STL-like stream to be used as input.
- allows case-insensitive parsing.
- allows for multiple errors per parse (error recovery).
- allows for parsing left-recursive grammars.
- provides facilities for creating Abstract Syntax Trees (ASTs) from parsing matches.

Version 1.0.0.7.

## Table Of Contents

 - [Using This Library](#using-this-library)
 - [Writing Grammars](#writing-grammars)
 	- [Terminals](#terminals)
 	- [Non-Terminals](#non-terminals)
 	- [Custom Parsing Functions](#custom-parsing-functions)
 	- [Expression](#expressions)
 	- [Rules](#rules)
 	- [Matches](#matches)
 - [Parsing Input](#parsing-input)
 	- [Basic Steps](#basic-steps)
 	- [Customizing Parsing](#customizing-parsing) 
 - [Error Handling](#error-handling)
 	- [Managing Errors](#managing-errors)
 	- [Multiple Errors Per Parse](#multiple-errors-per-parse)
 - [Case Insensitive Parsing](#case-insensitive-parsing)
 - [Custom Token Parsing](#custom-token-parsing)
 - [Parsing Streams](#parsing-streams)
 - [Counting Lines And Columns](#counting-lines-and-columns)
 	- [Counting Lines And Columns Over Strings](#counting-lines-and-columns-over-strings)
 	- [Counting Lines And Columns Over Streams](#counting-lines-and-columns-over-streams)
 - [Parsing UTF-8 Strings](#parsing-utf-8-strings)
 - [How Left-Recursion Parsing Works](#how-left-recursion-parsing-works)
 	- [Detecting Left Recursion](#detecting-left-recursion)
 	- [Parsing Left Recursion](#parsing-left-recursion)
 	- [Example](#example)
 	- [Associativity](#associativity)
 - [Creating ASTs](#creating-asts)
 - [Versions](#versions)
 - [TODO](#todo)

### Using This Library

The library is a header-only library. 

Include the file `parserlib.hpp` in order to use it.

Use `namespace parserlib` to access the library's symbols directly.

Example:

```cpp
#include "parserlib.hpp"
using namespace parserlib;
auto a = terminal('a');
```

### Writing grammars

#### Terminals

Terminals consist of:

- value terminals; for example, the character `a`.
- string terminals; for example, the string `abc`.
- range terminals; for example the range from `0` to `9`.
- set terminals; for example, one of `0`, `1`, `2`, `3`, `4`, `5`, `6`, `7`, `8`, `9`.

The library provides the following functions for creating terminal parsers:

- `terminal(value)`: creates a terminal value parser.
- `terminal((begin, end)|const T*|std::basic_string<T>|std::basic_string_view<T>)`: creates a terminal string parser.
- `range(min, max)`: creates a range parser.
- `one_of(begin, end)|const T*|container|initializer_list|tuple|T...)`: creates a set parser.
- `end`: used to test if the input was fully parsed.

Examples:

```cpp
//value terminal
auto term = terminal(';');

//string terminal
auto keyword = terminal("int");

//range terminal
auto digit = range('0', '9');

//set terminal
auto marker = one_of('x', 'y', 'z');

//end
auto some_grammar = terminal('a') >> end;
```

#### Non-Terminals

Non-terminals are:

- loops: zero or time times, one or more times.
- optionals.
- logical predicates: logical and, logical not.
- series: sequence, choice.

Examples:

```cpp
//parse a terminal zero or more times
*terminal('a');

//parse a terminal one or more times
+terminal('a');

//parse a terminal optionally
-terminal('a');

//logical and predicate;
//logical predicates do not consume input
&terminal('a');

//logical not predicate
!terminal('a');

//sequences
terminal('a') >> terminal('b') >> terminal("123");

//choices
terminal('a') | terminal('b') | terminal("123");
```

#### Custom Parsing Functions

The Parserlib function `function` creates a parser out of a function.

The function must have the following signature: `bool(ParseContext&)`.

Example:

```cpp
#include <cassert>
#include <cwctype>
#include "parserlib.hpp"

using namespace parserlib;

static bool test_isalpha(uint32_t v) {
    return std::iswalpha((wint_t)v);
}

static auto letter = function([](auto& context) {
    if (test_isalpha(*context.parse_position())) {
        context.increment_parse_position();
        return true;
    }
    return false;
    });

static auto digit = range('0', '9');

static auto identifier = letter >> *(letter | digit | '_');
```

#### Expressions

Terminals and non-terminals can be combined to form a grammar. Example:

```cpp
auto digit = range('0', '9');
auto lowercase_letter = range('a', 'z');
auto uppercase_letter = range('A', 'Z');
auto letter = lowercase_letter | uppercase_letter;
auto identifier = letter >> *(letter | digit | '-');
```

#### Rules

Rules are grammatic expressions that can be recursive. For example:

```cpp
const auto integer = +digit;
rule<> integer_list = integer >> ',' >> integer_list
                    | integer;
```

Rules can also be left-recursive. For example:

```cpp
const auto integer = +digit;
rule<> integer_list = integer_list >> ',' >> integer
                    | integer;
```

In order to implement recursion, rules require to use an interface internally to store the expression.

Therefore, the rule class must be parameterized by the parse context to use.

By default, a rule uses a parse context that parses std strings, but that can be changed to the parse context that is going to be used for parsing.

Grammars that are to be used with multiple parse contexts can be implemented using template structs. For example:

```cpp
template <class ParseDefinitions>
struct my_grammar {
	using ParseContext = parse_context<ParseDefinitions>;
    using rule = parserlib::rule<ParseContext>;
    
    rule integer_list;
    
    my_grammar() {
		const auto integer = +digit;
    	integer_list = integer_list >> ',' >> integer
                     | integer;
    }
};
```

#### Matches

In order to allow for recognition of a parsed entity, a match parser allows the addition of a match to the parse context, if another parser parses successfully. 

Matches can be retrieved by the member function `matches()` on the parse context.

For example:

```cpp
enum ID {
	A, B, C
}

//'a' -> 'A'
const auto a = terminal('a') ->* A;

//'b' -> 'B'
const auto b = terminal('b') ->* B;

//'c' -> 'C'
const auto c = terminal('c') ->* C;

const auto grammar = *(a | b | c);
std::string input = "abc";
parse_context<> context(input);
grammar.parse(context);
const auto matches = context.matches();
```

In the above grammar:
- when 'a' is parsed, a match with id = `A` will be added to the parse context;
- when 'b' is parsed, a match with id = `B` will be added to the parse context;
- when 'c' is parsed, a match with id = `C` will be added to the parse context.

### Parsing Input

#### Basic Steps

In order to parse an input, the user has to:

1. Prepare the grammar.
2. Prepare the input.
2. Prepare the appropriate parse context.
3. Invoke the root of the grammar with the prepared parse context.
4. Process the matches of the parse context after parsing ends.

Example:

```cpp
#include "parserlib.hpp"
using namespace parserlib;

//prepare the grammar.
auto digit = range('0', '9');
auto integer = +digit;
auto integer_list = integer >> *(',' >> integer);
auto grammar = integer_list >> end;

//prepare the input.
std::string input = "123";

//prepare the parse context.
parse_context<> context(input);

//parse.
const bool ok = grammar.parse(context);

//process matches
oonst auto matches = context.matches();
for(const auto& match : matches) {
   ...
}
```

#### Customizing Parsing

In order to customize parsing, the appropriate parse definitions type must be created and passed to the parse context.

The class `parse_definitions` has the following signature:

```cpp
template <
    class Input = std::string, 
    class OutputToken = int, 
    class ErrorId = int, 
    class TokenComparator = case_sensitive_comparator>
class parse_definitions;
```

a `parse_definitions` can be customized over the following:

- Input: the type of the container that contains the source; it must have an STL container-like interface.
- OutputToken: the type of the token produced when there is a match, i.e. the id of a match.
- ErrorId: the type of id of errors.
- TokenComparator: the comparator used to compare tokens and terminals; useful for parsing case-insensitive grammars.

After customizing the parse definitions, they need to be passed to the `parse_context` class. For example:

```cpp
using MyParseDefinitions = parse_definitions<MyContainer, MyTokenId, MyErrorID, MyTokenComparator>;
using MyParseContext = parse_context<MyParseDefinitions>;
```

### Error Handling

#### Managing Errors

By default, parsing stops at the first error encountered.

The function `parse_position()` can be used on a `parse context` to identify the position parsing stopped.

The function `first_unparsed_position()` can be used on a `parse context` to identify the maximum position the parser reached.

For example:

```cpp
const error_pos = context.parse_position();
const unparsed_pos = context.first_unparsed_position();
```

#### Multiple Errors Per Parse

Parserlib allows for parsing to continue after an error, providing various `error recovery` facilities:

- the function `on_error(parser, error_handler)` can be used to specify an error handler when the given `parser` fails to parse.
- the function `on_error_continue(parser, error_id, skip_error_func)` can be used to add an error with the given error id to the parse context, then continue parsing.
- the function `on_error_continue_after(parser, skip_error_token)` can be used to skip tokens until the `skip_error_token` is matched.

For example:

```cpp
//will continue parsing after ';', even if there is an error in a statement.
const auto statement_decl = ...;
const auto statement = on_error_continue_after(statement_decl, MyErrorID, ';');
```

Errors can be processed after parsing by calling the member function `errors()` on a `parse context`. For example:

```cpp
const auto errors = context.errors();
for(const auto& error : errors) {
   ...
}
```

### Case Insensitive Parsing

In order to parse a character input with case insensitivity, the `class parse_definitions` must be provided with the appropriate token comparator.

Parserlib provides the class `case_insensitive_comparator', which uses the c++ function `std:tolower` in order to compare tokens. For example:

```cpp
using ParseDefinitions = parse_definitions<std::string, int, int, case_insensitive_token_comparator>;
using ParseContext = parse_context<ParseDefinitions>;
```

### Custom Token Parsing

Parserlib allows for anything to be parsed, including a vector of custom structures.

The only requirement is to declare the appropriate subtraction operations between types. For example:

```cpp
#include "parserlib.hpp"
using namespace parserlib;

enum class source_token_id {
    A,
    B,
    C
};

struct token {
    source_token_id id;
    std::string source;
    std::size_t line;
    std::size_t column;
};

//subtraction operation needed between custom token and custom match id
static int operator - (const token& a, const source_token_id& b) noexcept {
    return static_cast<int>(a.id) - static_cast<int>(b);
}

enum class ast_token_id {
    A,
    B,
    C
};

void test_custom_token_parsing() {
    const auto grammar = +(
        (terminal(source_token_id::A) ->* ast_token_id::A) |
        (terminal(source_token_id::B) ->* ast_token_id::B) |
        (terminal(source_token_id::C) ->* ast_token_id::C)
    );

    using ParseDefinitions = parse_definitions<std::vector<token>, ast_token_id>;
    using ParseContext = parse_context<ParseDefinitions>;

    std::vector<token> source;
    source.push_back(token{ source_token_id::A, "a", 1, 1 });
    source.push_back(token{ source_token_id::B, "b", 1, 1 });
    source.push_back(token{ source_token_id::C, "c", 1, 1 });

    ParseContext context(source);
    assert(grammar.parse(context));

    assert(context.matches().size() == 3);
    assert(context.matches()[0].id() == ast_token_id::A);
    assert(context.matches()[1].id() == ast_token_id::B);
    assert(context.matches()[2].id() == ast_token_id::C);
}

```

### Parsing Streams

Parserlib provides the class 

```cpp
template <
	class Stream, 
    class Buffer = std::vector<typename Stream::char_type>, 
    std::size_t ReadAheadCount = 4096>
class stream_wrapper;
```

which can be used to wrap a stream in a string-like class `stream_wrapper`, which can be used as a source for parsing.

Example:

```cpp
const int DIGIT = 1;
const int NEWLINE = 2;

const auto digit = range('0', '9')->*DIGIT;
const auto line_end = terminal("\r\n")->*NEWLINE;
const auto grammar = *(digit | line_end);

using StreamWrapper = stream_wrapper<std::stringstream>;
using ParseDefinitions = parse_definitions<StreamWrapper>;
using ParseContext = parse_context<ParseDefinitions>;

std::string inputString = "123\r\n456\r\n789";
std::stringstream inputStream;
inputStream << inputString;
StreamWrapper source(inputStream);

ParseContext context(source);

assert(grammar.parse(context));
assert(context.is_end_parse_position());

assert(context.matches().size() == 11);
```

### Counting Lines And Columns

#### Counting Lines And Columns Over Strings

By default, `Parserlib` does not provide line and column information, when parsing text.

For that to happen, a string class must be wrapped into the following class:

```cpp
template <class String> class string_wrapper;
```

The `string_wrapper` class provides an iterator class that provides line and column information.

In order to count lines, the function `newline` should be used that increments the current line and sets the column to 0.

Example:

```cpp
const int DIGIT = 1;
const int NEWLINE = 2;

const auto digit = range('0', '9')->*DIGIT;
const auto line_end = newline("\r\n")->*NEWLINE;
const auto grammar = *(digit | line_end);

using StringWrapper = string_wrapper<std::string>;
using ParseDefinitions = parse_definitions<StringWrapper>;
using ParseContext = parse_context<ParseDefinitions>;

std::string input = "123\r\n456\r\n789";
StringWrapper source(input);

ParseContext context(source);

assert(grammar.parse(context));
assert(context.is_end_parse_position());

assert(context.matches().size() == 11);
```

#### Counting Lines And Columns Over Streams

In order to count lines and columns over streams, the class `stream_wrapper` must be combined with the class `string_wrapper`.

Example:

```cpp
const int DIGIT = 1;
const int NEWLINE = 2;

const auto digit = range('0', '9')->*DIGIT;
const auto line_end = newline("\r\n")->*NEWLINE;
const auto grammar = *(digit | line_end);

using StreamWrapper = stream_wrapper<std::stringstream>;
using StringWrapper = string_wrapper<StreamWrapper>;
using ParseDefinitions = parse_definitions<StringWrapper>;
using ParseContext = parse_context<ParseDefinitions>;

std::string inputString = "123\r\n456\r\n789";
std::stringstream inputStream;
inputStream << inputString;
StreamWrapper streamSource(inputStream);
StringWrapper source(streamSource);

ParseContext context(source);

assert(grammar.parse(context));
assert(context.is_end_parse_position());

assert(context.matches().size() == 11);
```

### Parsing UTF-8 Strings

In order to parse utf-8 strings, `Parserlib` provides the class

```cpp
template <class String = std::string> class utf8_string;
```

The class `utf8_string` provides the minimum functionality required for iterating over utf-8.

It can be customized over the string type that it uses for iterator, which means that:

- it can be combined with class `stream_wrapper` to provide parsing over utf-8 streams.
- it can be combined with class `string_wrapper` to provide line and column counting over utf-8 strings.
- it can be combined with class `string_wrapper<stream_wrapper>` to provide both stream parsing and lines/columns.

### How Left Recursion Parsing Works

`Parserlib` provides a parsing algorithm that can take care of left-recursive grammars.

Left-recursion parsing is done on rules (instances of the `rule` class), because that is the only class that allows recursion.

#### Detecting Left Recursion

Left-recursion is detected by keeping the last position a rule was called to parse.

If a rule is called to parse a position in the source again, then it is a left-recursive rule, and left-recursion parsing is activated (through the `class left_recursion` thrown as an exception, used as a non-local goto for simplicity).

#### Parsing Left Recursion

In order to parse left recursion, the left-recursive rule must first be deactivated, in order to parse the non-left-recursive parse of the grammar, and then it must be neutralized, in order to return 'true' without calling its internal parser, in order to parse the part of the grammar that comes after the left-recursive rule.

In other words, left-recursion parsing is done in two steps:

1. a non-left recursive part of the grammar must parse successfully (left recursion start).
2. the part after the recursive rule must parse successfully (left recursion continuation).

#### Example

Here is a calculator grammar that is left-recursive.

```cpp
auto digit = range('0', '9');

auto num = +digit >> -('.' >> +digit);

auto val = num
    | '(' >> add >> ')';

mul = mul >> '*' >> val
    | mul >> '/' >> val
    | val;

add = add >> '+' >> mul
    | add >> '-' >> mul
    | mul;
```

##### Step 1 - parsing left recursion start

In step 1, the rule `add` is deactivated and returns false, in order to parse the non-left-recursive parts of the grammar. The grammar can be viewed like this:

```cpp
add = REJECT(add) >> '+' >> mul
    | REJECT(add) >> '-' >> mul
    | mul;
```

In this way, parsing skips the left-recursive branches.

##### Step 2 - parsing left recursion continuation

In step 2, the rule `add` is neutralized and returns `true` without calling the internal parser, allowing the part after the rule to parse. The grammar can be viewed like this:

```cpp
add = ACCEPT(add) >> '+' >> mul
    | ACCEPT(add) >> '-' >> mul
    | REJECT(mul);
```

Then, by parsing the rule 'add' repeatedly, expressions like `5+3*2` can be parsed.

#### Associativity

The above algorithm maintains the left-associativity of the operations.

An expression like `5+3-2` is parsed as if it was `((5+3)-2)`, for example.

### Creating ASTs

`Parserlib` provides classes and functions to ease creation of Abstract Syntax Trees from matches created from parsing:

- `template <class ParseDefinitions> class ast_node`: base class for AST nodes.
- `template <class ASTNode> class ast_node_factory`: factory that allows creation of AST nodes.
- `template <class ParseDefinitions, class AstNodeFactory>
ast_node_ptr_type<ParseDefinitions> create_ast_node(const match<ParseDefinitions>& m[, const AstNodeFactory& ast_node_factory])`: creates a single AST node from a match.
- `template <class ParseDefinitions, class AstNodeFactory>
std::vector<ast_node_ptr_type<ParseDefinitions>> create_ast_nodes(const std::vector<match<ParseDefinitions>>& matches[, const AstNodeFactory& ast_node_factory])`: creates multiple AST nodes from an array of matches.

The ast node factory parameter is optional: if ommitted, an instance of `ast_node` will be created.

For a full example of using custom ast nodes, see [the relevant unit test](https://github.com/axilmar/parserlib/blob/master/tests/test_ast_node.cpp).

### Versions

  - 1.0.0.7
    - Rewritten from scratch, to improve quality of the API.

  - 1.0.0.6
  	- Added function-based parsing.

  - 1.0.0.5
  	- Added custom match functions in order to allow the resolution of ambiguities while parsing.
  	- allowed terminal values to be of different type that the value of the source container, in order to allow the result of a parse (the ast nodes created by a parse) to be fed to another parse function.
  	- added terminal parsing via functions.
  	- added parsing via standalone functions.
  	- added multiple error handling.

  - 1.0.0.4
  
    - Rewrote the library:
        - all parser grammar classes are now inside a single template class `class parser_engine<SourceT, MatchIdT>`, for the following reasons:
            - compiler performance (MSVC 32-bit regularly crashed with out of memory error from the many template instantiations of previous versions).
            - library code organization; writing a grammar usually requires including all the grammar constructs, so it is reduntant to have separate files for each grammar-related class.
            - user code organization; whole grammars need to be specialized on source type.
        - coding style is closer to the standard: all identifiers are lower case, words are separated by underscores, idiomatic c++ is used whenever possible.
    - Rewrote the documentation, due to more functionality to be added in the future.
 
 
  - 1.0.0.3
 
 	- Reorganized the library in order to support compiler front ends into a separate namespace. The main library is now in `namespace parserlib::core`.
 	- Added `namespace parserlib::cfe` which now contains the compiler-front-end functionality.
 	- separated tokenization and parsing phases for compiler-front-ends.
 	- Added relevant documentation and unit tests.
 

  - 1.0.0.2
 
 	- Rewrote the library from scratch in order to provide a better interface. Changes:
 	
        - All the getter methods now start with 'get', in order to play better with Intellisense.
        - The `ParseContext` class is now configured over the Source type, with the default class being the class `SourceString`.
        - The class `SourceString` provides custom iterator which counts lines and columns, compatible with the `std::string` interface.
        - The functions `terminal`, `terminalSet`, `terminalRange` are changed to `term`, `oneOf`, `oneIn`.
        - Matches are now only hierarchical (as in `operator >=` of previous version).
        - The `'operator >=` has been replaced with `operator ->*`, which is much more distinct than the former; no more typing accidentally '>>' where `>=` was intended.
        - The default match id type is no longer a string; it is an int.
        - Simplified the left recursion parsing implementation.
 
  - 1.0.0.1
 
  	- Added support for compiler front-end construction.
  	
- 1.0.0.0
 
  	- Initial release.

### TODO

TODO list in the current version:

 - review and improve the library documentation.
