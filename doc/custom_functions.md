# Parserlib

## Custom Parse Functions/Callbacks/Nodes

###### Table Of Contents

- [Custom Parse Functions](#custom-parse-functions)
- [Custom Parse Callbacks](#custom-parse-callbacks)
- [Custom Parse Nodes](#custom-parse-nodes)

### Custom Parse Functions

Parserlib allows any function to become a parse node, via the function `function`.

The function signature must be the following:

```cpp
template <class ParseContext>
parse_result my_function(ParseContext& pc) noexcept;
```

For example, using the function `std::isalnum` to test if a character is alphanumeric, a parse function can be written like this:

```cpp
const auto letter = function([](auto& pc) -> parse_result {
	if (pc.is_valid_parse_position() && std::isalpha(*pc.parse_position())) {
    	pc.increment_parse_position();
    	return true;
    }
    return false;
});;

const auto letter_or_digit = function([](auto& pc) -> parse_result {
	if (pc.is_valid_parse_position() && std::isalnum(*pc.parse_position())) {
    	pc.increment_parse_position();
    	return true;
    }
    return false;
});

const auto identifier = letter >> *(letter_or_digit | '_');
```

### Custom Parse Callbacks

Sometimes it is needed to process the result of a grammatic expression with a function. In order to achieve this, parserlib allows the declaration of a callback function as an array index on a grammar expression.

The signature of the callback shall be the following:

```cpp
template <Class ParseContext, class Iterator>
parse_result my_callback(ParseContext& pc, const parse_result& result, const Iterator& start, const Iterator& end) noexcept;
```

Parameters:

- `pc`: the current parse context.
- `result`: result of the grammar expression that the callback is for.
- `start`: start position in the source.
- `end`: end position in the source.

A callback can be combined [with a parse context extension](./parse_context.md#extending-a-parse-context) in order to provide useful side effects when parsing a grammar.

Example:

```cpp
template <Class ParseContext, class Iterator>
parse_result my_callback(ParseContext& pc, const parse_result& result, const Iterator& start, const Iterator& end) noexcept {
	pc.extension().identifiers.insert(source(start, end));
    return result;
}

const auto identifier = (letter >> *(letter_or_digit | '_'))[identifier_callback];
```

### Custom Parse Nodes

In order to create a custom parse node class, the class `parse_node<Derived>` must be subclassed.

There are 3 functions that can optionally be specified in the subclass. The class `parse_node<Derived>` provides the default implementations:

```cpp
template <class ParseContext>
parse_result parse(ParseContext& pc) const noexcept {
	throw std::logic_error("Parse method not specified.");
}

template <class ParseContext>
parse_result parse_left_recursion_start(ParseContext& pc) const noexcept {
    return false;
}

template <class ParseContext, class State>
parse_result parse_left_recursion_continuation(ParseContext& pc, const State& match_start) const noexcept {
    return false;
}
```

The 3 functions to respecify in the subclass are the following:

1) `parse(ParseContext& pc)`: invoked at the normal (i.e. non-left recursive) parse flow; it must be respecified by subclasses, otherwise an exception will be thrown (and since the method is noexcept, it will terminate the program).

2) `parse_left_recursion_start(ParseContext& pc)`: optional method that is invoked as the first parsing step after left recursion is detected; left recursive rules always return `false` at this point, to avoid reentering an infinite loop.

3) `parse_left_recursion_continuation(ParseContext& pc, const State& match_start)`: optional method that is invoked to parse after the first step when left recursion is detected; left-recursive rules always return true at this point, in order to allow the expressions that they are the first members of to continue parsing.

[Example of specifying a custom parse node.](./terminals.md#custom-terminals)
