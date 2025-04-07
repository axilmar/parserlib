# Parserlib

## Recursion

### The `rule` Class

The class `rule<class ParseContext>` allows the declaration of recursive grammars.

Within a grammar expression, rules are kept by reference, allowing rules to be used before they are constructed.

For example:

```cpp
rule<MyParseContext> rule1 = terminal('a') >> rule1;
```

### Left Recursive Grammars

The class `rule` allows the implementation of left-recursive grammars.

For example:

```cpp
rule<MyParseContext> rule1 = rule1 >> b' | 'a';
```

The above grammar parses strings like "a", "ab", "abb", "abbb" etc.

Left-associativity is preserved.

### How Left Recursion Works

It is often mentioned that recursive-descent parsers cannot handle left-recursion grammars.

This library can do it, in three steps:

1. recognizing left recursion.
2. parsing the first token after a left recursion is detected (left recursion start).
3. parsing the rest of the tokens after the first token is parsed (left recursion continuation).

#### Recognizing Left Recursion

The class `parse_context` provides functions for allowing a rule to detect when it is left recursive:

- `is_left_recursive_rule(rule)`: checks if the parse position pushed in an internal stack is the same as the current parse position.
- `push_rule_parse_position(rule)`: pushes the current parse position for the given rule into an internal stack.
- `pop_rule_parse_position(rule)`: pops a parse position from the internal parse position stack for a rule.

With the above functions, a rule can check if it is left recursive in the current parse position.

### Parsing The Left Recursion Start

After recognizing that a rule ls left recursive, parsing should continue with left recursive rules denying parsing, in order to avoid infinite recursion.

In order to achieve that, every parse node contains the function `parse_left_recursion_start(parse context)`, which allows rules to return false if they are left recursive at the current parse position.

Terminal expressions parse normally in this state, allowing sequences and branches that are not left recursive to successfully complete parsing.

For example, the above-mentioned grammar, when in the left recursion start state, is equal to the following grammar:

```cpp
rule<MyParseContext> rule1 = terminal(false) >> 'b' | 'a';
```

This allows 'a' to be parsed and avoid the infinite recursion.

### Parsing The Left Recursion Continuation

After the first token is parsed successfully under left recursion parsing, the whole left-recursive tree should again be parsed repeatedly, but this time the left recursive rules should return true without parsing, in order to allow left-recursive branches to parse through.

In this state, terminals should not parse, in order to avoid non-left recursive branches to be parsed.

In order to achieve that, every parse node contains the function `parse_left_recursion_continuation(parse context, match_start)`, which allows rules to return true if they are left recursive at the current parse position, and terminals to return false.

The above function also allows match parsers to consider the start of the left-recursive match to be the one when left recursion parsing started, instead of the current parse state.

For example, the above-mentioned grammar, when in the left recursion continuation state, is equal to the following grammar:

```cpp
rule<MyParseContext> rule1 = *(terminal(true) >> 'b') | terminal(false) >> 'a';
```

This allows the part `>> 'b'` to parse after `'a'` is parsed, allowing `'ab'` to be parsed.

Parsing a single `'a'` is prohibited.

And then the part `>> 'b'` is called again to parse after `'ab'` is parsed, allowing `'abb'` to be parsed, ad infinitum.

The loop stops until no more parsing can be done in the continuation phase.

#### Sequences

When a sequence is parsed, the left recursion continuation state is active only for the first member of the sequence.
After parsing the first member, the sequence calls the normal function `parse(pc)` for all the subsequent parse nodes.

Therefore, left recursive grammars should NOT be like this:

```cpp
rule<MyParseContext> rule1 = rule1 >> rule1 >> b' | 'a';
```

The above will not work, since the second invocation of `rule1` will successfully parse a left recursion, but then it will cause the sequence to fail when it returns true, because the next token will not be a `'b'` (if it was, it would have been consumed by left recursion parsing).

