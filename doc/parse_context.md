# Parserlib

## The Parse Context

The class [`parse_context`](../include/parserlib/parse_context.hpp#parse_context) plays a pivotal role in parsing:

- It holds all the state and functions needed for parsing.
- It is used by grammar expressions to implement their parsing algorithm.
- It contains the matches (the portions of the source matched by the grammar expressions).
- It contains the errors placed by the error constructs that might exist in a grammar tree.

###### Table Of Contents

1. [Constructing And Using A Parse Context](#constructing-and-using-a-parse-context)
1. [Customizing The Character Comparison Function](#customizing-the-character-comparison-function)
1. [Extending A Parse Context](#extending-a-parse-context)
1. [Processing The Results Of Parsing](#processing-the-results-of-parsing)
    1. [Processing The Parse Result](#processing-the-parse-result)
    1. [Processing Matches After A Parse](#processing-matches-after-a-parse)
    1. [Processing Errors After A Parse](#processing-errors-after-a-parse)
4. [Using A Parse Context Inside A Parse Function](#using-a-parse-context-inside-a-parse-function)

### Constructing And Using A Parse Context

In order to construct a parse context, the following items are needed:

1. the type of the source.
2. the type of the enumeration that contains the ids of matches.
3. the type of the enumeration that contains the ids of errors.
4. the type of the class that implements character comparison.
5. an optional extension class used to hold custom data.

The signature of the class is the following:

```cpp
    template <
    	class Source, 
        class MatchId = int, 
        class ErrorId = int, 
        class Comparator = case_sensitive_comparator, 
        class Extension = empty_parse_context_extension>
    class parse_context;
```

Parsing requires the following steps:

1. declaring the source.
2. declaring a parse context for the source.
3. invoking a grammar with the parse context.

For example:

```cpp
#include <string>
#include "parserlib.hpp"
using namespace parserlib;

enum class MATCH_ID {
    A,
    B,
    C
};

enum class ERROR_ID {
    ERROR_1,
    ERROR_2
};

static const auto a = terminal('a')->*MATCH_ID::A;
static const auto b = terminal('b')->*MATCH_ID::B;
static const auto c = terminal('b')->*MATCH_ID::C;
static const auto grammar = a >> b >> c;

int main() {
    std::string source = "abc";
    parse_context<std::string, MATCH_ID, ERROR_ID> pc(source);
    parse_result result = grammar.parse(pc);
}
```

### Customizing The Character Comparison Function

The template parameter `Comparator` of the class `parse_context` is the type of the class that will be used for character comparison. The comparator class must have the following member function:

```cpp
int operator ()(const A& a, const B& b) const noexcept;
```

The operator must return one of the following:

- a value less than 0, if `a < b`.
- 0, if `a == b`.
- a value greater than 0, if `a > b`.

The library already provides two relevant comparator implementations:

- `class case_sensitive_comparator`: it simply returns `a - b`; this is the default comparator.
- `class case_insensitive_comparator`: it returns `std::tolower(a) - std::tolower(b)`, making the comparison case-insensitive.

The `parse_context` class includes the static function `int compare(a, b)` which invokes the comparator defined as a template parameter.

### Extending A Parse Context

Sometimes it is useful to be able to extend a parse context, for example with a lookup table.

The template parameter `Extension` contains the extension type. The default is the `class empty_parse_context_extension` which is empty.

When constructing a parse context instance, an instance of the extension can be optionally passed to the constructor. Here is an example:

```cpp
class my_extension : public parserlib::parse_context_extension_base {
    std::set<std::string> identifiers;
};

int main() {
    std::string source = "abc";
    parse_context<std::string, MATCH_ID, ERROR_ID, case_sensitive_comparator, my_extension> pc(source, my_extension());
    parse_result result = grammar.parse(pc);
}
```

The grammar expressions will get the above version of the parse context, which can be accessed via the member function `extension()`.

### Processing The Results Of Parsing

#### Processing The Parse Result

Invoking a grammar's `parse` function (with a parse context) parses the input and returns the result of parsing. 

The result of parsing is captured in the class [`parse_result`](../include/parserlib/parse_node.hpp#parse_result), which provides the following interface:

```cpp
//check if parsing failed
bool is_false() const noexcept;

//check if parsing succeeded
bool is_true() const noexcept;

//check if parsing failed due to left recursion
bool is_left_recursion() const noexcept;

//allows `if (result)`; true only when `is_true()` returns true.
operator bool() const noexcept;

//possible result values
enum value_type {
    FALSE = 0,
    TRUE = 1,
    LEFT_RECURSION = 2
};

//returns the current value
value_type value() const noexcept;
```

Example:

```cpp
int main() {
    std::string source = "abc";
    parse_context<std::string, MATCH_ID, ERROR_ID> pc(source);
    parse_result result = grammar.parse(pc);
    if (result.value() == parse_result::FALSE) {
    	std::cout <<< "failed to parse!.";
    }
    else if (result.value() == parse_result::TRUE) {
    	std::cout <<< "parsing successful.";
    }
    else {
    	std::cout <<< "failed to parse due to unresolved left recursion.";
    }
}
```

#### Processing Matches After A Parse

After parsing, the matches can be processed by looping over the matches of a parse context. Example:

```cpp
int main() {
    std::string source = "abc";
    parse_context<std::string, MATCH_ID, ERROR_ID> pc(source);
    grammar.parse(pc);
    for(const auto& match : pc.matches()) {
    	std::cout << "found match: " <<< match.id();
    	std::cout << "; source:" << match.source();
        std::cout << std::endl;
    }
}
```

Matches have children. The children can be processed by calling the function `children()` on a match. Example:

```cpp
template <class Match> void print_match(const Match& match) noexcept;
template <class Matches> void print_matches(const Matches& matches) noexcept;

template <class Match>
static void print_match(const Match& match) noexcept {
    std::cout << "found match: " <<< match.id();
    std::cout << "; source:" << match.source();
    std::cout << std::endl;
    print_matches(match.children());
}

template <class Matches> 
static void print_matches(const Matches& matches) noexcept {
    for(const auto& match : matches) {
    	print_match(match);
    }
}

int main() {
    std::string source = "abc";
    parse_context<std::string, MATCH_ID, ERROR_ID> pc(source);
    grammar.parse(pc);
    print_matches(pc.matches());
}
```

#### Processing Errors After A Parse

In the same manner matches can be processed after a parse, so can errors. Example:

```cpp
int main() {
    std::string source = "abc";
    parse_context<std::string, MATCH_ID, ERROR_ID> pc(source);
    grammar.parse(pc);
    for(const auto& error : pc.errors()) {
    	std::cout << "found error: " <<< error.id();
    	std::cout << "; source:" << error.source();
        std::cout << std::endl;
    }
}
```

### Using A Parse Context Inside A Parse Function

Assuming one wants to write a custom parse function, a `parse_context` instance contains numerous functions that can assist in parsing:

TODO
