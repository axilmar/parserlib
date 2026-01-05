# Parserlib

## How left recursion parsing works

`Parserlib` is a recursive-descent parsing library that supports left recursion.

The algorithm used is very simple: when a left recursion is detected, parsing restarts from the top left-recursive rule, and executed in two phases:

1) in the **reject** phase, left-recursive rules return false, to allow the non-left-recursive parts of the grammar to parse.
2) in the **accept** phase, left-recursive rules return true without parsing, to allow next parse nodes to parse. 

The accept phase is repeated in a loop, until no more parsing can be done.

### How left-recursion is detected

A parse context can keep the parse position (i.e. iterator) where a rule is called to parse from; if the new position is equal to the previous position, i.e. if there is no progress between the last time a rule was called to parse and the current time, then there is a left recursion.

When a left recursion is detected, a left-recursive rule throws a `left_recursion_exception` exception.

Then this exception is caught from a higher scope and the left recursion parsing algorithm is used.

If the left recursion cannot be solved, then it propagates to the user's code.

### The reject phase

In the reject phase, a left-recursive rule just returns false, forcing evaluation of branches that are not left-recursive.

For example, if we have the grammar:

```cpp
rule list_a 
    = list_a >> 'a' 
    | 'a'
    ;
```

Then in the reject phase, the grammar can be thought of as:

```cpp
rule list_a 
    = false >> 'a' 
    | 'a'
    ;
```

Thus allowing the branch 'a' to be evaluated first.

### The accept phase

In the accept hase, a left recursive rule returns true, without parsing, in a loop, allowing the next parse nodes to parse.

For example, the above grammar can be thought of, in the accept phase, as:

```cpp
rule list_a 
    = *(true >> terminal('a')) 
    | 'a'
    ;
```

### Left-associativity is maintained

The library takes special care of matches, in order to allow left-associativity to be maintained when parsing a left recursion.

A parse context contains a separate parse state for matches, which is always rewinded at the start of each accept phase loop to the point where the left recursion started, thus maintaining left associativity.

When a token is consumed, the match parse state of a parse context is set to the current parse state, thus advancing both parse states together.
