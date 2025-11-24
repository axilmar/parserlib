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
* debugging via annotations.

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

### Using a parser

In order to use a parser, a parse context must be initialized from an input (or an input range), then passed to the parser.

Example:

```cpp
std::string input = "123";
parse_context pc{ input };
//alternatively
//parse_context pc{ input.begin(), input.end() };
const bool result = parser.parse(pc);
```

#### The `parse_context` class

The class `parse_context` plays the most important role in this library, as it is used to keep the parsing state, which includes:

* current parse position
* match stack
* error stack
* rule state (used in left recursion)
* methods to test and modify its state

The class `parse_context` provides an API for retrieving and setting the above state, and that API is used by parse nodes.

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

By default, the type `default_source_type` is `std::string`.

Example:

```cpp
std::vector<point> points;
parse_context pc{ points; };
```

###### Parameter 'MatchId'

The `MatchId` parameter of a `parse_context` class represents the id type of a match.

It can be anything, including class enumerations.

By default, the type `default_match_id_type` is `int`.

Example:

```cpp
enum class my_match_id { ID1, ID2, ID3 };
parse_context<std::string, my_match_id> pc{ src; };
```

###### Parameter 'ErrorId'

The `ErrorId` parameter of a `parse_context` class represents the id type of an error.

It can be anything, including class enumerations.

By default, the type `default_error_id_type` is `int`.

Example:

```cpp
enum class my_error_id { ERROR1, ERROR2, ERROR3 };
parse_context<std::string, int, my_error_id> pc{ src; };
```

###### Parameter 'TextPosition'

The `TextPosition` parameter of a `parse_context` class represents the type of the position within the text.

A text position type must have the following interface:

```cpp
struct a_text_position {
    size_t line() const;
    size_t column() const;
    void increment_column();
    void increment_column(size_t count)
    void increment_line();
    std::string to_string() const;
};
```

By default, the type `default_text_position_type` is `default_text_position`, which implements an empty text position.

The class `text_position` implements the above interface by maintaining line and column counters, and therefore can be used to report line and column for character parsers.

Example:

```cpp
parse_context<std::string, int, int, text_position> pc{ src; };
```

###### Parameter 'SymbolComparator'

The `SymbolComparator` parameter of a `parse_context` class represents the type of the comparator for symbols.

A symbol comparator must have the following interface:

```cpp
struct symbol_comparator {
	template <class A, class B>
    int operator ()(A, B) const;
}
```

The `operator ()` of the comparator must return the difference between the given values:

* if A < B, a negative value
* if A == B, zero
* if A > B, a positive value

By default, the type `default_symbol_comparator_type` is `default_symbol_comparator`, which has the following implementation:

```cpp
class default_symbol_comparator {
public:
    template <class A, class B>
    int operator ()(const A& a, const B& b) const {
        return static_cast<int>(a) - static_cast<int>(b);
    }
};
```

The class `case_sensitive_symbol_comparator` can be used for case-insensitive comparisons. Its implementation is:

```cpp
class case_insensitive_symbol_comparator {
public:
    template <class A, class B>
    int operator ()(int a, int b) const {
        return std::tolower(static_cast<int>(a) - std::tolower(static_cast<int>(b));
    }
}
```

i.e. it uses the function `std::tolower` to convert both symbols to lower case in order to compare them.

Example:

```cpp
parse_context<std::string, int, int, default_text_position_type, case_sensitive_symbol_comparator> pc{ src; };
```

###### Parameter 'Extensions'

The `Extensions` parameter of a `parse_context` class is a list of classes that `parse_context` inherits from, allowing extending the parse context with new capabilities.

For example, one could implement a symbol lookup table extension:

```cpp
class symbol_lookup_extension {
public:
	std::string get_symbol(int symbol_id) const;
    void add_symbol(int symbol_id, const std::string& value);
};

parse_context<std::string, int, int, default_text_position_type, default_symbol_comparator_type, symbol_lookup_extension> pc{ src; };
```

##### Using a parse context

The following sections describe the parse context API.

###### The constructor

A parse context can be constructed in two ways:

1. from an iterator range of an STL container.
2. from an STL container.

Both constructors optionally can receive arguments for the extensions.

Examples:

```cpp
	//without arguments for extension constructors.
	parse_context<> pc1(str);
	parse_context<> pc1(str.begin(), str.end());
    
    //with arguments to extension constructors.
	parse_context<> pc1(str, val1);
	parse_context<> pc1(str.begin(), str.end(), val1);
```

###### Method 'parse_position()`

The method `parse_position()` returns the current parse position.

A parse position has the following interface:

```cpp
struct parse_position {
	iterator() const;
    text_position() const;
    increment();
    increment(count);
    increment_line();
    to_string(source begin_iterator) const;
};
```

###### Method 'iterator()'

The method `iterator()` returns the iterator of the parse position:

```cpp
//get symbol to parse
*pc.iterator();
```

###### Method `text_position()`

The method `text_position()` returns the current text position:

```cpp
std::cout << "current text position is: " << pc.text_position().to_string();
```

###### Method 'begin_iterator()'

The method `begin_iterator()` returns the iterator from which parsing started:

```cpp
pc.begin_iterator();
```

###### Method 'end_iterator()'

The method `end_iterator()` returns the iterator that is the end of the parse input:

```cpp
pc.end_iterator();
```

###### Method 'begin()'

The method `begin()` returns the iterator from which parsing started:

```cpp
pc.begin();
```

###### Method 'end()'

The method `end()` returns the iterator that is the end of the parse input:

```cpp
pc.end();
```

###### Method 'parse_valid()'

The method `parse_valid()` checks if the end is reached; it returns:

* `true` if the end is not reached yet.
* `false` if the end is reached.

```cpp
if (pc.parse_valid()) {
	...
}
```

###### Method 'parse_ended()'

The method `parse_ended()` checks if the end is reached; it returns:

* `true` if the end is reached.
* `false` if the end is not reached yet.

```cpp
if (!pc.parse_ended()) {
	...
}
```

###### Method 'increment_parse_position()'

The method `increment_parse_position()` increments the current parse position by one.

It increments the current iterator by one, and the text position by one column.

It does not check if end has been reached in release mode, for performance reasons.

Incrementing the parse position beyond the end of the source is an undefined operation.

```cpp
pc.increment_parse_position();
```

###### Method 'increment_parse_position(count)'

The method `increment_parse_position(count)` increments the current parse position by the given count.

It increments the current iterator by the given count, and the text position by the given amount of columns.

It does not check if end is reached in release mode, for performance reasons.

Incrementing the parse position beyond the end of the source is an undefined operation.

```cpp
pc.increment_parse_position(10);
```

###### Method 'increment_parse_position_line()'

The method `increment_parse_position_line()` increments the current text position line by one.

It does not increment the current iterator.

```cpp
pc.increment_parse_position_line();
```

###### Method 'match_count()'

The method `match_count()` returns the number of matches in the match stack.

```cpp
pc.match_count();
```

###### Method 'matches()'

The method `matches()` returns the stack of matches.

```cpp
pc.matches();
```

###### Method 'get_match_start_state()'

The method `get_match_start_state()` returns the state of the parse context for the current match start.

The match start state differs from the current state, for left recursion parsing.

This function returns the appropriate start state for matches for both cases (left and non-left recursion).

```cpp
pc.get_match_start_state();
```

###### Method 'add_match(id, start_state, end_iterator)'

the method `add_match(id, start_state, end_iterator)` adds a match to the match stack of a parse context.

```cpp
pc.add_match(IDENTIFIER, match_start_state, pc.iterator());
```

###### Method 'error_count()'

The method `error_count()` returns the number of errors in the error stack.

```cpp
pc.error_count();
```

###### Method 'errors()'

The method `errors()` returns the stack of errors.

```cpp
pc.errors();
```

###### Method 'add_error(id, start_pos, end_iterator)'

The method `add_error(id, start_pos, end_iterator)` adds an error to the error stack of a parse context.

```cpp
pc.add_error(SYNTAX_ERROR, start_pos, end_it);
```

###### Method 'get_state()'

The method `get_state()` returns the current state of a parse context.

A state has the following methods:

* parse_position()
* iterator()
* text_position()
* match_count()

These methods are the same as the relevant methods of the parse context.

A state value can only be created by a parse context, via `get_state()`.

```cpp
pc.get_state();
```

###### Method 'set_state(state)'

The method `set_state(state)` sets the state of a parse context.

It is used to restore a parse context to a previous state.

```cpp
pc.set_state(prev_state);
```

###### Method 'get_error_state()'

The method `get_error_state()` returns the current error state of a parse context.

An error state has the following methods:

* error_count()

An error state value can only be created by a parse context, via `get_error_state()`.

```cpp
pc.get_error_state();
```

###### Method 'set_error_state(error_state)'

The method `set_error_state(state)` sets the state of a parse context.

It is used to restore a parse context's error state to a previous state.

```cpp
pc.set_error_state(prev_error_state);
```

###### Method 'int compare_symbols(A, B)'

The method `int compare_symbols(A, B)` compares two symbols and returns their numeric difference. The result can be:

* result < 0, i.e. A is less than B
* result > 0, i.e. A is greater than B
* result == 0, i.e. A is equal to B

A and B must be convertible to `int`, since the parse context will do a `static_cast<int>(a)` and `static_cast<int>(b)` to them respectively.

```cpp
int result = pc.compare(symbol1, symbol2);
```

###### Method 'int compare_current_symbol(T)'

The method `int compare_current_symbol(T)` compares the given symbol to the current symbol to parse, using the method `compare_symbols()`.

```cpp
int result = pc.compare_current_symbol(symbol1);
```

###### Method 'terminal_parsing_allowed()'

The method `terminal_parsing_allowed()` checks if a terminal can be parsed at the current parse context state.

It must be invoked for terminal parsers in order to not allow them to parse while expecting to accept a rule for left recursion.

```cpp
bool result = pc.terminal_parsing_allowed();
```

###### Method 'left_recursion_start_state()'

The method `left_recursion_start_state()` returns the start state for the current left recursion.

The start state of a left recursion is always the point in the source that the left recursion started, while the current parse state can be furher ahead than the left recursion start state.

This allows for maintaining left-associativity, when parsing left recursions.

```cpp
auto state = pc.left_recursion_start_state();
```

###### Method 'set_left_recursion_start_state(state)'

The method `set_left_recursion_start_state(state)` sets the left recursion start state.

```cpp
pc.set_left_recursion_start_state(state);
```

###### Method 'derive_parse_context < DerivedMatchId, DerivedErrorId >()'

The method `derive_parse_context < DerivedMatchId, DerivedErrorId >()` creates and returns a new parse context, that can parse the matches of the source parse context.

This allows the result of one parsing stage (usually, the tokenizing stage) to be parsed immediately.

Example:

```cpp
auto parser_parse_context = pc.derive_parse_context<PARSE_MATCH_ID, PARSE_ERROR_ID>();
parser_grammar.parse(parser_parse_context);
```

For a full example of this, [see the test function `test_multistage_parsing()`](./tests/tests.cpp#test_multistage_parsing).

##### Method 'parse(parse_node | expression)'

The methodd `parse(parse_node | expression)` parses a parse node; if an expression is given, then it is converted to a parse node.

In debug mode, it calls the `init` method of a parse node, which fills the `m_text` members of parse nodes of the tree.

Example:

```cpp
const auto grammar = terminal("abc");
std::string src = "abc";
parse_context<> pc(src);
pc.parse(grammar);
```

#### Using the parsing results

The member function `parse(parse_context)` of a parse node returns a bool which indicates if the parsing was successful.

Besides checking that result, there are some other items that a parse context provides in order to check if parsing was successful:

1. checking if the whole input was parsed, by using the parse context function `parse_ended()`.
2. checking the matches, by using the parse context function `matches()`.
3. checking the errors, by using the parse context function `errors()`.

##### Using matches

The `matches()` function of a parse context returns the matches found during parsing. It returns an `std::vector` of `match` objects.

Each match object has the following interface:

* method `id()`: returns the id of the match.
* method `parse_position()`: returns the start parse position of the match.
* method `end_iterator()`: returns the end iterator of the match.
* method `begin()`: returns the start parse position of the match, which also can be used as a const iterator.
* method `end()`: returns the end iterator of the match.
* method `children()` or `matches()`: returns the children matches.
* method `source()`: returns the source portion of the input that corresponds to the match; it is either a string, for character-based input, or a vector, for non-character-based input.
* method `visit(const auto& visitor, size_t depth = 0)`: can be used to visit the match and its children.

**NOTE**: *The end position of parsing is not a parse position, because it may not be a parse position, it may be the end of the input.*

Example:

```cpp
bool result = grammar.parse(pc);
assert(result);
assert(pc.parse_ended());
for(const auto& match : pc.matches()) {
	switch (m.id()) {
    	case ...: std::cout << m.source() << std::endl;
        ....
    }
}
```

##### The 'infinite_recursion_exception'

When it is impossible to parse an input due to infinite recursion, an infinite recursion exception is thrown.

Here is an example of how to handle it:

```cpp
try {
    bool ok = grammar.parse(pc);
}
catch (const infinite_recursion_exception& ex) {
	std::cut << ex.what() << std::endl;
}
```

Since parse nodes have a text representation, a possible output can be:

```cpp
infinite recursion exception at line 5, column 2, from 'rule1'
```

##### Using errors

The `errors()` function of a parse context returns the errors found during parsing. It returns an `std::vector` of `parse_error` objects.

Each error object has the following interface:

* method `id()`: returns the id of the error.
* method `parse_position()`: returns the start parse position of the error.
* method `end_iterator()`: returns the end iterator of the error.
* method `begin()`: returns the start parse position of the error, which also can be used as a const iterator.
* method `end()`: returns the end iterator of the error.
* method `source()`: returns the source portion of the input that corresponds to the error; it is either a string, for character-based input, or a vector, for non-character-based input.

Example:

```cpp
bool result = grammar.parse(pc);
assert(result);
assert(pc.parse_ended());
for(const auto& error : pc.errors()) {
	std::cout << "error: " << error.id() << " at position " << error.parse_position().to_string(input.begin());
}
```

**NOTE**: *`input.begin()` is passed to the `parse_position().to_string()` function in order to get the index of the iterator, if the text position is empty; the index is written to the string, instead of the text position.*

##### How errors work

The `error` function can be used to add an error to the parse context, which contains an error container (a vector of error instances).

The error container is not reset to a previous state when the parse context state is rewinded to a previous state, because errors should remain for the user to see them; that is the reason there are separate `get_error_state()` and `set_error_state` functions in a parse context.

The library keeps the errors of the longest parsing; i.e. if we have the following grammar:

```cpp
enum { EXPECTED_B, EXPECTED_C };

const auto term
    = terminal('a') >> ('b' | (error(EXPECTED_B) >> false)) >> ('c' | error(EXPECTED_C))
    | terminal('a') >> ('b' | error(EXPECTED_B))
    ;

const auto grammar = term;
```

Then the following errors will be created, for the given inputs:

| input  | error        | result | input exhausted            |
|--------|--------------|--------|----------------------------|
| "abc"  | none         | true   | yes                        |
| "ab"   | none         | true   | yes                        |
| "ab@"  | EXPECTED_C   | true   | no; parsing stopped at '@' |
| "a@"   | EXPECTED_B   | true   | no; parsing stopped at '@' |
---------------------------------------------------------------

The result of parsing 

#### Abstract Syntax Trees (ASTs)

While matches can be processed 'as-is', it is often desirable to convert the match tree to an abstract syntax tree.

The following functions can be used to convert matches to ASTs:

* function `make_ast(match, ast_node_factory)`: creates one ast node from one match, including their children, using the given ast node factory.
* function `make_ast(match)`: creates one ast node from one match, including their children, using the default ast node factory.
* function `make_ast(matches, ast_node_factory)`: creates a vector of ast nodes from a vector of matches, including their children, using the given ast node factory.
* function `make_ast(matches)`: creates a vector of ast nodes from a vector of matches, including their children, using the default ast node factory.

An `ast node factory` shall have the method `make_node(match)` which returns a shared pointer to the created ast node from a match.

##### The `ast_node` class

The `ast_node` class has the following signature:

```cpp
template <class Source = std::string, class MatchId = int, class TextPosition = default_text_position> class ast_node;
```

And the following interface:

* method `id()`: returns the id of the ast node.
* method `parse_position()`: returns the start parse position of the part of the source the ast node represents.
* method `end_iterator()`: returns the end iterator of the part of the source the ast node represents.
* method `begin()`: returns the start parse position of the part of the source that the ast node represents, which also can be used as a const iterator.
* method `end()`: returns the end iterator of the part of the source the ast node represents.
* method `source()`: returns the source portion of the input the ast node represents; it is either a string, for character-based input, or a vector, for non-character-based input.
* method `content()`: returns the original source that was parsed.
* method `parent()`: returns a pointer to the parent ast node.
* method `children()`: returns a vector of children.
* method `add_child(child, index = -1)`: adds a child at the specified index; -1 means 'last child'.
* method `remove_child(child)`: removes a child.
* method `remove_all_children()`: removes all children.
* method `visit(const auto& visitor, size_t depth = 0)`: can be used to visit the AST node and its children.

**NOTE**: *AST nodes are managed via shared pointers.*

Example:

```cpp
const auto ast_nodes = make_ast(pc.matches());
for(const auto &ast_node : ast_nodes) {
	std::cout << ast_node->id() << std::endl;
}
```

### Debugging a parser

#### Debugging via `debug`

The `debug` function wraps a parse node into an instance of `debug_parse_node`, which can be used to set a breakpoint into.

For example:

```cpp
const auto c = ...;
const auto expression = a >> b >> debug(c) >> d >> e;
```

By wrapping `c` into a `debug` parser, and setting up a breakpoint here ([file debug_parse_node.hpp](include/parserlib/debug_parse_node.hpp)):

```cpp
template <class ParseContext>
bool parse(ParseContext& pc) const {
    //put breakpoint here
    const bool result = m_child.parse(pc);
    return result;
}

```

one can debug a particular part of a grammar (`c` in the above example).

#### Debugging via annotations

The `operator []` can be used on parse nodes to add an annotation to them.

An annotation is any value the user wants; for debugging, it usually is the name/role of an expression.

By default, annotations are not processed; an extension is required that provides the function `parse_annotation(parse_context&, parse_node, annotation)`.

`Parserlib` already provides two such implementations:

1. `class default_debug_annotations_extension`: an empty implementation of the debug annotations extension.
2. `class debug_annotations_extension<OutputStream = std::ostream>`: an implementation of the debug annotations extension which prints the annotations as a tree, in an output stream of choice.

The type alias `default_debug_annotations_extension_type` defaults to `class default_debug_annotations_extension`.

Here is an example:

```cpp
int main() {
	const auto a = terminal('a')["A"];
	const auto c = terminal('c')["C"];
	const auto d = terminal('d')["D"];
	const auto b = (c | d)["B"];
	const auto grammar = (*(a | b))["Grammar"];
    
    std::string source = "aca";
    
    parse_context<
    	std::string, 
        int, 
        int, 
        default_text_position_type, 
        default_symbol_comparator_type, 
        debug_annotations_extension<>>
        pc(source);
    
    grammar.parse(pc);
}
```

The above code produces the following output:

```
Parsing Grammar at index 0
    Parsing A at index 0
    Successfully parsed A
    Parsing A at index 1
    Failed to parse A
    Parsing B at index 1
        Parsing C at index 1
        Successfully parsed C
    Successfully parsed B
    Parsing A at index 2
    Successfully parsed A
Successfully parsed Grammar
```

The default output stream for annotations is `std::cout` (or `std::wcout` if the type of output stream is `std::wostream`), but it can be customized:

1. by setting the output stream with the function `pc.set_output_stream(&stream)`.
2. by setting the template output stream type of the class `debug_annotations_extension`.

Examples:

```cpp
//set the output stream to be a file
std::ofstream file_stream(...);
pc.set_output_stream(&file_stream);

//declare another parse context which uses wchar_t for annotations:
parse_context<
	std::string, 
    int, 
    int, 
    default_text_position_type, 
    default_symbol_comparator_type, 
    debug_annotations_extension<std::wostream> 
    pc1(source);
```

#### Inspecting the field 'm_text'.

Each parse node has a field `m_text` which is set to the textual description of the parse node.

This is valid only in debug mode.

For example, some of the JSON parser texts are:

```
//text for 'object' expression
((LEFT_CURLY_BRACKET >> -(object_member >> *(COMMA >> object_member)) >> RIGHT_CURLY_BRACKET) ->* OBJECT)

//text for 'value' expression
(string | number | object | array | true | false | null)
```

## Parsing left recursive grammars

`Parserlib` can handle left recursion grammars.

The way this is achieved is the following:

* a parse context keeps the last parse position used by a rule, and therefore can recognize when left recursion exists dynamically: when the new parse position for a rule is the same as the previous one, then there is a left recursion on that rule.
* upon recognizing left recursion, the parse context is rewinded to the last non-left recursive position for that rule. Then left recursion parsing starts, and it is executed in two phases:
	1. the `reject` phase, in which left recursive rules reject parsing in order to allow non-left recursive rules to parse.
	2. the `accept` phase, in which left recursive rules accept parsing without calling the parser, allowing the parts after the left recursive rules to be parsed.

The `accept` phase loops until no more parsing can be done.

The algorithm keeps the left-associativity of the grammar, by forcing matches to start from where the left recursion starts.

Let's see an example. Here is a left-recursive calculator grammar:

```cpp
const auto digit = range('0', '9');

const auto num = +digit >> -('.' >> +digit);

rule<> val = num
           | '(' >> add >> ')';

rule<> mul = mul >> '*' >> val
           | mul >> '/' >> val
           | val;

rule<> add = add >> '+' >> mul
           | add >> '-' >> mul
           | mul;
```

In the `reject` phase, the above grammar looks like this:

```cpp
const auto digit = range('0', '9');

const auto num = +digit >> -('.' >> +digit);

rule<> val = num
           | '(' >> add >> ')';

rule<> mul = REJECT >> '*' >> val
           | REJECT >> '/' >> val
           | val;

rule<> add = REJECT >> '+' >> mul
           | REJECT >> '-' >> mul
           | mul;
```

This allows the non-left recursive parts to be parsed first.

Then, in the `accept` phase, the grammar looks like this:

```cpp
const auto digit = range('0', '9');

const auto num = +digit >> -('.' >> +digit);

rule<> val = num
           | '(' >> add >> ')';

rule<> mul = *(ACCEPT >> '*' >> val
           | ACCEPT >> '/' >> val
           | REJECT);

rule<> add = *(ACCEPT >> '+' >> mul
           | ACCEPT >> '-' >> mul
           | REJECT);
```

The above algorithm allows for left-recursive grammars to be parsed.

## Tokenization and parsing

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

##### The grammar

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

#### Tokenizing and parsing with one function

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

#### Calculating the result

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

## Changes

- 1.0.0.9
	- Rewritten again from scratch in order to improve the quality.
	- changes:
		- renamed some functions in order to make more sense when the code is read.
		- improved support for catching errors and continuing.
		- more analytical documentation.
	- new features:
		- better debugging support.
		- grammar annotations.
		- extensible parse context.
		- infinite recursion exception.
		- loop breaks.
		- compile-time rule optimizations.

- 1.0.0.8
	- Rewritten again from scratch, in order to deal with error handling in a much better way,
  	  to make everything `noexcept` for increased performance,
      to add new capabilities.

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