# Parserlib

## Using a parser

* [defining a parser type](#defining-a-parser-type)
* [Using a parse context](#using-a-parse-context)
* [Parsing streams](#parsing-streams)
* [The parse_context class](#the-parse_context-class)
* [Case-insensitive parsing](#case-insensitive-parsing)
* [Counting lines and columns](#counting-lines-and-columns)
* [Processing matches](#processing-matches)
* [Processing errors](#processing-errors)

### Defining a parser type.

The first step towards parsing is to define a parser.

A `parser` instance has the following signature:

```cpp
template <class Iterator = std::string::const_iterator, class MatchId = int, class ErrorId = int, class SymbolComparator = default_symbol_comparator> class parser;
```

The template parameters of the class are:

- `Iterator`: the iterator type; can be an STL container iterator or an STL stream iterator or anything that implements an iterator interface.
- `MatchId`: type of the match id; each time a match is found, this id, along with the match range are stored into the parse context.
- `ErrorId`: type of the error id; each time an error is found, this id, along with the error range are stored into the parse context.
- `SymbolComparator`: type of class used for comparing tokens; it must have a function `compare` the following signature:

```cpp
//returns the numeric difference of two elements.
static int compare(A, B);
```

A good practice is to use a `typedef` or a `using` declaration for the required parser type, since every operation that needs a function to be called lies inside the `parser<>` class.

Example:

```cpp
using p = parser<>;
```

### Using a parse context

In order to parse a source, a parse context must be initialized with the source.

Example:

```cpp
	auto grammar = ...;
	std::string source = ...;
    p::parse_context pc(source);
    grammar.parse(pc);
```

A parse context can also be initialized with an iterator pair:

```cpp
	auto grammar = ...;
	std::string source = ...;
    p::parse_context pc(source.begin(), source.end());
    grammar.parse(pc);
```

### Parsing streams

The iterators can also be stream iterators, if parsing directly from a stream is required:

```cpp
    using p = parser<std::istream_iterator>;
	auto grammar = ...;
	std::stringstream source = ...;
    p::parse_context pc(std::istream_iterator(source), std::istream_iterator());
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

Usually, the match id and error id parameters can be enumerations.

### Case-insensitive parsing

The parameter `SymbolComparator` can be set to the class `case_sensitive_symbol_comparator` for case-insensitive parsing:

```cpp
parser<std::string::const_iterator, int, int, case_insensitive_symbol_comparator>::parse_context pc(source);
```

The class `case_sensitive_symbol_comparator` uses the function `std::tolower` to compare symbols in lower case.

### Counting lines and columns

In order to know on which line and column of a source a particular construct is, the special class `text_iterator` can be used:

```cpp
template <class Iterator> class text_iterator;
```

For example, in order to parse some text with line and column, the following parse context can be used:

```cpp
using source_iterator_type = std::string::const_iterator;
using iterator_type = text_iterator<source_iterator_type>;
using parse_context_type = parser<iterator_type>::parse_context;
```

The iterator type that is used for the `text_iterator` class can also be an `std::istream_iterator`.

### Processing matches

After parsing, the matches identified during parsing can be processed like this:

```cpp
for(const auto& match : pc.get_matches()) {
    std::cout << match.get_id() << std::endl; //get the id
    std::cout << match.get_source() << std::endl; //get source
    std::cout << match.begin().get_line() << std::endl; //get the begin iterator line (if `get_line()` is applicable)
    std::cout << match.begin().get_column() << std::endl; //get the begin iterator column (if `get_column()` is applicable)
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
