# Parserlib

## Using a parser

* [Using a parser context](#using-a-parser-context)
* [The parse_context class](#the-parse_context-class)
* [Customizing a parse context](#customizing-a-parse-context)
	* Parameters
        * [Source](#parameter-'source')
        * [MatchId](#parameter-'matchid')
        * [ErrorId](#parameter-'errorid')
        * [TextPosition](#parameter-'textposition')
        * [SymbolComparator](#parameter-'symbolcomparator')
        * [Extensions](#parameter-'extensions')
* [Using a parse context](#using-a-parse-context)
	* [Constructors](#the-constructor)
	* Methods
		* Getters
            * [parse_position](#method-'parse_position')
            * [iterator](#method-'iterator')
            * [text_position](#method-'text_position')
            * [begin_iterator](#method-'begin_iterator')
            * [end_iterator](#method-'end_iterator')
            * [begin](#method-'begin')
            * [end](#method-'end')
            * [parse_valid](#method-'parse_valid')
            * [parse_ended](#method-'parse_ended')
        	* [match_count](#method-'match_count')
        	* [matches](#method-'matches')
        	* [error_count](#method-'error_count')
            * [errors](#method-'errors')
            * [get_state](#method-'get_state')
            * [get_error_state](#method-'get_error_state')
            * [compare_symbols](#method-'compare_symbols')
            * [compare_current_symbol](#method-'compare_current_symbol')
            * [terminal_parsing_allowed](#method-'terminal_parsing_allowed')
            * [left_recursion_start_state](#method-'left_recursion_start_state')
            * [derive_parse_context](#method-'derive_parse_context')
        	* 
		* Setters/Modifiers
            * [increment_parse_position](#method-'increment_parse_position')
            * [increment_parse_position_line](#method-'increment_parse_position_line')
            * [get_match_start_state](#method-'get_match_start_state')
            * [add_match](#method-'add_match')
            * [add_error](#method-'add_error')
            * [set_state](#method-'set_state')
            * [set_error_state](#method-'set_error_state')
            * [set_left_recursion_start_state](#method-'set_left_recursion_start_state')
            * [parse](#method-'parse')
            * [add_errors](#method-'add_errors')
            * [set_parse_position_after_last_error](#method-'set_parse_position_after_last_error')
    * Types
    	* [state](#type-'state')
    	* [error_state](#type-'error_state')
* [Using the parsing results](#using-the-parsing-results)
	* [Matches](#matches)
	* [The infinite recursion exception](#the-infinite-recursion-exception)
	* [Errors](#errors)
	* [Abstract Syntax Trees](#abstract-syntax-trees)

### Using a parser context

In order to use a parser, a parse context must be initialized from an input (or an input range), then passed to the parser.

Example:

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

###### Method 'parse_position'

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

###### Method 'iterator'

The method `iterator()` returns the iterator of the parse position:

```cpp
//get symbol to parse
*pc.iterator();
```

###### Method 'text_position'

The method `text_position()` returns the current text position:

```cpp
std::cout << "current text position is: " << pc.text_position().to_string();
```

###### Method 'begin_iterator'

The method `begin_iterator()` returns the iterator from which parsing started:

```cpp
pc.begin_iterator();
```

###### Method 'end_iterator'

The method `end_iterator()` returns the iterator that is the end of the parse input:

```cpp
pc.end_iterator();
```

###### Method 'begin'

The method `begin()` returns the iterator from which parsing started:

```cpp
pc.begin();
```

###### Method 'end'

The method `end()` returns the iterator that is the end of the parse input:

```cpp
pc.end();
```

###### Method 'parse_valid'

The method `parse_valid()` checks if the end is reached; it returns:

* `true` if the end is not reached yet.
* `false` if the end is reached.

```cpp
if (pc.parse_valid()) {
	...
}
```

###### Method 'parse_ended'

The method `parse_ended()` checks if the end is reached; it returns:

* `true` if the end is reached.
* `false` if the end is not reached yet.

```cpp
if (!pc.parse_ended()) {
	...
}
```

###### Method 'increment_parse_position'

The method `increment_parse_position()` increments the current parse position by one.

It increments the current iterator by one, and the text position by one column.

It does not check if end has been reached in release mode, for performance reasons.

Incrementing the parse position beyond the end of the source is an undefined operation.

```cpp
pc.increment_parse_position();
```

The method `increment_parse_position(count)` increments the current parse position by the given count.

It increments the current iterator by the given count, and the text position by the given amount of columns.

It does not check if end is reached in release mode, for performance reasons.

Incrementing the parse position beyond the end of the source is an undefined operation.

```cpp
pc.increment_parse_position(10);
```

###### Method 'increment_parse_position_line'

The method `increment_parse_position_line()` increments the current text position line by one.

It does not increment the current iterator.

```cpp
pc.increment_parse_position_line();
```

###### Method 'match_count'

The method `match_count()` returns the number of matches in the match stack.

```cpp
pc.match_count();
```

###### Method 'matches'

The method `matches()` returns the stack of matches.

```cpp
pc.matches();
```

###### Method 'get_match_start_state'

The method `get_match_start_state()` returns the state of the parse context for the current match start.

The match start state differs from the current state, for left recursion parsing.

This function returns the appropriate start state for matches for both cases (left and non-left recursion).

```cpp
pc.get_match_start_state;
```

###### Method 'add_match'

the method `add_match(id, start_state, end_iterator)` adds a match to the match stack of a parse context.

```cpp
pc.add_match(IDENTIFIER, match_start_state, pc.iterator());
```

###### Method 'error_count'

The method `error_count()` returns the number of errors in the error stack.

```cpp
pc.error_count();
```

###### Method 'errors'

The method `errors()` returns the stack of errors.

```cpp
pc.errors();
```

###### Method 'add_error'

The method `add_error(id, start_pos, end_iterator)` adds an error to the error stack of a parse context.

```cpp
pc.add_error(SYNTAX_ERROR, start_pos, end_it);
```

###### Method 'get_state'

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

###### Method 'set_state'

The method `set_state(state)` sets the state of a parse context.

It is used to restore a parse context to a previous state.

```cpp
pc.set_state(prev_state);
```

###### Method 'get_error_state'

The method `get_error_state()` returns the current error state of a parse context.

An error state has the following methods:

* error_count()

An error state value can only be created by a parse context, via `get_error_state()`.

```cpp
pc.get_error_state();
```

###### Method 'set_error_state'

The method `set_error_state(state)` sets the state of a parse context.

It is used to restore a parse context's error state to a previous state.

```cpp
pc.set_error_state(prev_error_state);
```

###### Method 'compare_symbols'

The method `int compare_symbols(A, B)` compares two symbols and returns their numeric difference. The result can be:

* result < 0, i.e. A is less than B
* result > 0, i.e. A is greater than B
* result == 0, i.e. A is equal to B

A and B must be convertible to `int`, since the parse context will do a `static_cast<int>(a)` and `static_cast<int>(b)` to them respectively.

```cpp
int result = pc.compare(symbol1, symbol2);
```

###### Method 'compare_current_symbol'

The method `int compare_current_symbol(T)` compares the given symbol to the current symbol to parse, using the method `compare_symbols()`.

```cpp
int result = pc.compare_current_symbol(symbol1);
```

###### Method 'terminal_parsing_allowed'

The method `terminal_parsing_allowed()` checks if a terminal can be parsed at the current parse context state.

It must be invoked for terminal parsers in order to not allow them to parse while expecting to accept a rule for left recursion.

```cpp
bool result = pc.terminal_parsing_allowed();
```

###### Method 'left_recursion_start_state'

The method `left_recursion_start_state()` returns the start state for the current left recursion.

The start state of a left recursion is always the point in the source that the left recursion started, while the current parse state can be furher ahead than the left recursion start state.

This allows for maintaining left-associativity, when parsing left recursions.

```cpp
auto state = pc.left_recursion_start_state();
```

###### Method 'set_left_recursion_start_state'

The method `set_left_recursion_start_state(state)` sets the left recursion start state.

```cpp
pc.set_left_recursion_start_state(state);
```

###### Method 'derive_parse_context'

The method `derive_parse_context < DerivedMatchId, DerivedErrorId >()` creates and returns a new parse context, that can parse the matches of the source parse context.

This allows the result of one parsing stage (usually, the tokenizing stage) to be parsed immediately.

Example:

```cpp
auto parser_parse_context = pc.derive_parse_context<PARSE_MATCH_ID, PARSE_ERROR_ID>();
parser_grammar.parse(parser_parse_context);
```

For a full example of this, [see the test function `test_multistage_parsing()`](./tests/tests.cpp#test_multistage_parsing).

###### Method 'parse'

The method `parse(parse_node | expression)` parses a parse node; if an expression is given, then it is converted to a parse node.

In debug mode, it calls the `init` method of a parse node, which fills the `m_text` members of parse nodes of the tree.

Example:

```cpp
const auto grammar = terminal("abc");
std::string src = "abc";
parse_context<> pc(src);
pc.parse(grammar);
```

###### Method 'add_errors'

The method `add_errors(error_container_type)` adds a container of errors to the errors of a parse context.

Example:

```cpp
pc.add_errors(other_errors);
```

###### Method 'set_parse_position_after_last_error'

The method `set_parse_position_after_last_error()` sets the parse position to be after the last error in the parse context's error container, if it is not empty.

It is useful in continuing parsing after the last error.

###### Type 'state'

the `state` member type of the `parse_context` class is an opaque type that allows retrieving and restoring a parse context's state via the functions `get_state` and `set_state` respectively.

The parse context state includes:

* the parse position.
* the match count.

###### Type 'error_state'

The `error_state` member type of the `parse_context` class is an opaque type that allows retrieving and restoring a parse context's error state via the functions `get_error_state` and `set_error_state` respectively.

The error state is manipulated separately from the general state of a parse context in order to allow more specialized error state handling. For example, the class `choice_parse_node` keeps the furthest errors between the errors produced by branches, i.e. it keeps the errors from the branch that parsed the most input.

The parse context error state includes:

* The error count.

#### Using the parsing results

The member function `parse(parse_context)` of a parse node returns a bool which indicates if the parsing was successful.

Besides checking that result, there are some other items that a parse context provides in order to check if parsing was successful:

1. checking if the whole input was parsed, by using the parse context function `parse_ended()`.
2. checking the matches, by using the parse context function `matches()`.
3. checking the errors, by using the parse context function `errors()`.

##### Matches

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

##### The infinite recursion exception

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

##### Errors

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

###### How errors work

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

##### Abstract Syntax Trees

While matches can be processed 'as-is', it is often desirable to convert the match tree to an abstract syntax tree.

The following functions can be used to convert matches to ASTs:

* function `make_ast(match, ast_node_factory)`: creates one ast node from one match, including their children, using the given ast node factory.
* function `make_ast(match)`: creates one ast node from one match, including their children, using the default ast node factory.
* function `make_ast(matches, ast_node_factory)`: creates a vector of ast nodes from a vector of matches, including their children, using the given ast node factory.
* function `make_ast(matches)`: creates a vector of ast nodes from a vector of matches, including their children, using the default ast node factory.

An `ast node factory` shall have the method `make_node(match)` which returns a shared pointer to the created ast node from a match.

###### The `ast_node` class

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

