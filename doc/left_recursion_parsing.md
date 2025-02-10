# Parserlib

## Left Recursion Parsing

### The Problem

Recursive-descent parsers cannot parse left-recursive grammars, because entering a parsing function that immediately calls itself creates an infinite loop.

For example, if we have the following grammar:

```
add = add + mul
    | add - mul
    | mul
        
mul = mul * num
    | mul / num
    | num
```        

Then if we had an `parse_add` function, it may have looked like this:

```cpp
bool parse_add(parsing_context& c) {
	if (parse_add(c) && parse_terminal(c, '+') && parse_mul(c))) {
        ...
    }
    else if (parse_add(c) && parse_terminal(c, '-') && parse_mul(c))) {
        ...
    }
    else {
    	return parse_mul(c);
    }
}
```

However, the above is left-recursive on two `if` blocks, and will crash the process if run.

### Solution

The above grammar would be written with Parserlib like this:

```cpp
    rule mul = mul >> '*' >> num 
             | mul >> '/' >> num
             | num;

	rule add = add >> '+' >> mul
             | add >> '-' >> mul
             | mul;
```

Parserlib would create the following object tree, for example for 'add':

```
add
  |-> &add -> terminal('+') -> &mul
  |-> &add -> terminal('-') -> &mul
  |-> &mul    
```

Left recursion can be parsed in two steps:

1. let the rule 'add' reject the parsing if left recursion is found; this is called the REJECT phase.
2. let the rule 'add' accept the parsing without advancing the parser input; this is called the ACCEPT phase.

#### The REJECT phase

In the REJECT phase, left-recursive rules are rejected. The object tree can be thought of like this (`&` denotes `reference to`, rules are always by reference to allow recursion, red denotes rejected parts, green denotes accepted parts):

```
add
   $${\color{red}|-> reject(&add) -> terminal('+') -> &mul}$$
   $${\color{red}|-> reject(&add) -> terminal('-') -> &mul}$$
   $${\color{green}|-> &mul}$$
```

So, when parsing, for example, `1 + 2`, the add rule becomes the same as the rule `mul`.

If the rule `mul` passes, then the parser enters the ACCEPT phase.

#### The ACCEPT phase

During the ACCEPT phase, the object tree looks like this:

```
add
   |-> loop
   |   $${\color{green}|-> accept(&add) -> terminal('+') -> &mul}$$
   |   $${\color{green}|-> accept(&add) -> terminal('-') -> &mul}$$
   |-> $${\color{red}reject(&mul)}$$
```

The left recursive rule itself is ignored, as as that `+ 2 - 3 + 4...` is parsed, until no more parsing can be done.

