# Parserlib

A c++17 recursive-descent parser library that can parse left-recursive grammars.

Example:

```cpp
extern Rule<> add;

const auto val = (+terminalRange('0', '9'));

const auto num = val
               | terminal('(') >> add >> terminal(')');

Rule<> mul = (mul >> terminal('*') >> num)
           | (mul >> terminal('/') >> num)
           | num;

Rule<> add = (add >> terminal('+') >> mul)
           | (add >> terminal('-') >> mul)
           | mul;
```

The above grammar is a direct translation of the following left-recursive EBNF grammar:

```
add = add + mul
    | add - mul
    | mul
    
mul = mul * num
    | mul / num
    | num
    
num = val
    | ( add )
    
val = digit+
```

