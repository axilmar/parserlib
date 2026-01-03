# Parserlib

## Using a parser

* [Using a parse context](#using-a-parse-context)
* [Parsing streams](#parsing-streams)
* [The parse_context class](#the-parse_context-class)
* [Case-insensitive parsing](#case-insensitive-parsing)
* [Counting lines and columns](#counting-lines-and-columns)
* [Processing matches](#processing-matches)
* [Processing errors](#processing-errors)

### Using a parse context

In order to parse a source, a parse context must be initialized with the source.

Example:

```cpp
	auto grammar = ...;
	std::string source = ...;
    parse_context<> pc(source);
    grammar.parse(pc);
```

A parse context can also be initialized with an iterator pair:

```cpp
	auto grammar = ...;
	std::string source = ...;
    parse_context<> pc(source.begin(), source.end());
    grammar.parse(pc);
```

### Parsing streams

The iterators can also be stream iterators, if parsing directly from a stream is required:

```cpp
	auto grammar = ...;
	std::stringstream source = ...;
    parse_context<std::istream_iterator> pc(std::istream_iterator(source), std::istream_iterator());
    grammar.parse(pc);
```

### The parse_context class

The parse_context class has the following signature:

```cpp
template <
    class Iterator = typename std::string::const_iterator,
    class MatchId = int,
    class ErrorId = int,
    class SymbolComparator = default_symbol_comparator
>
class parse_context;
```

It can be customized over the iterator type that is used for the source; the iterator type is also used for the matches and errors.

It can also be customized over the match id, error id and symbol comparator.

Usually, the match id and error id parameters are enumerations.

### Case-insensitive parsing

The parameter `SymbolComparator` can be set to the class `case_sensitive_symbol_comparator` for case-insensitive parsing:

```cpp
parse_context<std::string::const_iterator, int, int, case_insensitive_symbol_comparator> pc(source);
```

The class `case_sensitive_symbol_comparator` uses the function `std::tolower` to compare symbols in lower case.

### Counting lines and columns

In order to know on which line and column of a source a particular construct is, the special class `parse_iterator` can be used:

```cpp
template <class Iterator, class TextPosition> class parse_iterator;
```

As a text position, the class `file_text_position` can be used which maintains `column` and `line` properties.

For example, in order to parse some text with line and column, the following parse context can be used:

```cpp
using source_iterator_type = std::string::const_iterator;
using iterator_type = parse_iterator<source_iterato_type, file_text_position>;
using parse_context_type = parse_context<iterator_type>;
```

The iterator type that is used for the `parse_iterator` class can also be an `std::istream_iterator`.

### Processing matches

After parsing, the matches identified during parsing can be processed like this:

```cpp
for(const auto& match : pc.get_matches()) {
    std::cout << match.get_id() << std::endl; //get the id
    std::cout << match.get_source() << std::endl; //get source
    std::cout << match.begin().get_line() << std::endl; //get the begin iterator line (if `get_line()` is applicable)
    std::cout << match.end() << std::endl; //get the end iterator
    print_children(match.get_children()); //process children
}
```

### Processing errors

The error identified during parsing can be processed like this:

```cpp
for(const auto& error : pc.get_errors()) {
    std::cout << error.get_id() << std::endl; //get the id
    std::cout << error.get_source() << std::endl; //get source
    std::cout << error.begin() << std::endl; //get the begin iterator
    std::cout << error.end() << std::endl; //get the end iterator
}
```
