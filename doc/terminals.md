# Parserlib

## Terminals

###### Table Of Contents

- [Terminal Types](#terminal-types)
- [Character Terminals](#character-terminals)
- [String Terminals](#string-terminals)
- [Boolean Terminals](#boolean-terminals)
- [Set Terminals](#set-terminals)
- [Range Terminals](#range-terminals)
- [Custom Terminals](#custom-terminals)

### Terminal Types

The following data can be terminal types:

- integral types (characters, integers, floats, booleans, etc)
- strings (null terminated strings, `std::basic_string`)
- sets
- ranges
- any other value not inheriting from class `parse_node`.

### Character Terminals

Characters are automatically promoted to terminal values, when part of expressions.

Otherwise, the function `terminal` can be used to promote a value to a terminal. 

Examples:

```cpp
terminal('a');
terminal(32);
'a' >> b >> 'c';
```

### String Terminals

Null-terminated strings and `std::basic_string` instances are automatically promoted to string terminals, when part of expressions.

Otherwise, the function `terminal` can be used to promote a string to a terminal. 

Examples:

```cpp
terminal("abc");
std::string str1 = "123";
terminal(str1);
"a" >> b >> "c";
```

### Boolean Terminals

The values `false` and `true` are automatically promoted to boolean parse nodes, when part of expressions.

Otherwise, the function `terminal` can be used to promote a boolean to a terminal. 

Boolean terminals are useful in returning `false` or `true` on some grammar expressions.

Examples:

```cpp
terminal(false);
terminal(true);
"a" >> b >> false;
```

### Set Terminals

Values of type `std:set` and `std::unordered_set` are automatically promoted to set parse nodes, when part of expressions.

Otherwise, the function `terminal` can be used to promote a set to a terminal. 

Additionally, the parserlib function `set` can be used to directly create a set, out of values.

Parsing a set means to check if the token at the current position is within the set.

Examples:

```cpp
terminal(std::set({'a', 'b', 'c'}));
set('a', 'b', 'c');
set("abc");
```

### Range Terminals

The function `range` can be used to create a range of values to check the current token against.

Examples:

```cpp
const auto digits = range('0', '9');
const auto lower_case_letters = range('a', 'z');
```

### Custom Terminals

Any value not being a parse node will become a terminal automatically within an expression, or via the function `terminal`.

If there is no need for a special parse node, then nothing else is required.

If, however, a special parse node is required, then a special parse node class must be created, and the function `get_parse_node_wrapper` must be overriden to return an instance of that special parse node class.

For example:

```cpp
struct my_special_value {
};

class my_special_parse_node : public parserlib::parse_node<my_special_parse_node> {
public:
	my_special_parse_node(const my_special_value& v) noexcept {
    	//TODO
    }

	template <class ParseContext> 
    parse_result parse(ParseContext& pc) const noexcept {
    	//TODO
    }
    
	template <class ParseContext> 
    parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
    	//TODO
    }
    
	template <class ParseContext, class State> 
    parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
    	//TODO
    }
};

my_special_parse_node get_parse_node_wrapper(const my_special_value& v) noexcept {
	return v;
}

const auto grammar = my_special_value() >> terminal('a');
```
