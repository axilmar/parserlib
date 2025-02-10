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

In the REJECT phase, left-recursive rules are rejected. The object tree can be thought of like this (`&` denotes `reference to`, rules are always by reference to allow recursion):

```
add
   |-> REJECT(&add) -> terminal('+') -> &mul
   |-> REJECT(&add) -> terminal('-') -> &mul
   |-> &mul
```

So, when parsing, for example, `1 + 2`, the add rule becomes the same as the rule `mul`.

REJECTing a rule means to return false and reject the parsing.

If the rule `mul` passes, then the parser enters the ACCEPT phase.

#### The ACCEPT phase

During the ACCEPT phase, the object tree looks like this:

```
add
   |-> loop
   |   |-> ACCEPT(&add) -> terminal('+') -> &mul
   |   |-> ACCEPT(&add) -> terminal('-') -> &mul
   |-> REJECT(&mul)
```

The left recursive rule itself is ignored, as as that `+ 2 - 3 + 4...` is parsed, until no more parsing can be done.

ACCEPTing a rule means to return true withouth advancing the parse position.

### Matches

While during left recursion parsing, matches in the grammar tree are executed at the end of each matched sentence, maintaining associativty.

For example, a Parserlib grammar with matches would look like this:

```cpp
rule mul = (mul >> '*' >> num) ->* TokenType::Mul
         | (mul >> '/' >> num) ->* TokenType::Div
         | num;

rule add = (add >> '+' >> mul) ->* TokenType::Add
         | (add >> '-' >> mul) ->* TokenType::Sub
         | mul;
```

And the object tree would look like this:

```
add
  |-> match(TokenType::Add)
  |    |-> &add -> terminal('+') -> &mul
  |-> match(TokenType::Sub)
  |    |-> &add -> terminal('-') -> &mul
  |-> &mul    
```

During the REJECT phase, the object tree shall look like this:

```
add
  |-> match(TokenType::Add)
  |    |-> REJECT(&add) -> terminal('+') -> &mul
  |-> match(TokenType::Sub)
  |    |-> REJECT(&add) -> terminal('-') -> &mul
  |-> &mul    
```

During the ACCEPT ptas, the object tree shall look like this:
```
add
  | -> loop
  |   |-> match(TokenType::Add)
  |   |    |-> ACCEPT(&add) -> terminal('+') -> &mul
  |   |-> match(TokenType::Sub)
  |   |    |-> ACCEPT(&add) -> terminal('-') -> &mul
  |-> &mul    
```

