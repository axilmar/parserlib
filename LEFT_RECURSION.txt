This document explains how left recursion PEG parsing works.

A. THE ALGORITHM
----------------

STEP 1 : recognizing left recursion.
------------------------------------

Left recursion is recognized by counting how many symbols have been parsed from the last
invocation of a rule. If the number is 0, i.e. if no symbols are parsed between two successive
invocations of a rule, then we have left recursion.

This trick works for both direct and indirect left recursion.

STEP 2 : dealing with left recursion.
-------------------------------------

Once left recursion is recognized, dealing with it has three steps:

a. selecting a non-left-recursive branch of the rule to parse.
b. looping until no more left recursion parsing can be done.
c. abandoning any more parsing in the sequence that the left recursion was recognized into.

STEP 2.1 : selecting a non-left recursive branch of the rule.
-------------------------------------------------------------

Let's assume that our grammar is:

    expr = expr '+' int
         | expr '-' int
         | int
         
In order to select a branch of the above rule that is non-left recursive,
we have to view the grammar as if it was this:

    expr = int
    
In order to do that, we have to stop the parser from going into the left-recursive branches.

We have to deactivate these two branches:

    expr '+' int
    expr '-' int

We do that by using a flag in the rule object, which tells us that upon the next invocation,
the rule must fail.

Indeed, if we do that, our grammar becomes:

    expr = FAIL(expr) '+' int
         | FAIL(expr) '-' int
         | int

So this trick allows us to select 'int' as a replacement for 'expr', when parsing "expr '+' int".

We can then continue parsing with "'+' int".

STEP 2.2 : looping until no more left recursion parsing can be done.
--------------------------------------------------------------------

The previous step is only enough to parse a single instance of left recursion. Once we have parsed
a rule, we need to loop over the rule in order to continue parsing, because there might be more
left recursion instances.

If we loop on the grammar as it is after step 2.1, the branches "FAIL(expr) '+' int" 
and "FAIL(expr) '-' int" will always fail. 

We could remove the FAIL part, i.e. we could reset the 'fail' flag of the rule 
and retry the parse on this grammar:

    expr = expr '+' int
         | expr '-' int
         | int

If we did that, then we wouldn't be able to parse left recursion any more, 
because after an 'int' is parsed, what follows is '+' or '-', and not another 'int'.

In order to continue with left recursion, we need to view the grammar like this:

    expr = '+' int
         | '-' int
         | int

In order to do that, we put another flag in the rule, which instructs the algorithm
to accept the rule without parsing it. We convert the grammar to this:

    expr = ACCEPT(expr) '+' int
         | ACCEPT(expr) '-' int
         | int

We can then invoke the rule again, and parse either a '+', a '-' or an 'int'.
We put that in a loop, until no more parsing can be done.

STEP 2.3 : abandoning any more parsing in the sequence that the left recursion was recognized into.
---------------------------------------------------------------------------------------------------

The left recursion, in the above example, was found in the following sequence:

    expr '+' int
    
Since the part "'+' int" has already been parsed by descenting into the rule,
we don't need to parse it again. So, the parse function must return not only 'true' or 'false',
but also the special value 'left recursion', in order to avoid continuing parsing the sequence.

If 'true' is returned, then the sequence "'+' int" will be parsed, something that is invalid.
If 'false' is returned, parsing will fail, although it did not.

In my implementation, I have chosen to exit the function via an exception, but I could just easily
avoid the exception if I changed all my parser objects to return 'left recursion' additionally to 'true/false'.

B. EXAMPLE
----------

Suppose our input is:

    1 + 2 + 3
    
And our grammar is:

    expr = expr '+' int
         | expr '-' int
         | int

The execution of the algorithm is:

    parse "1 + 2 + 3" with rule "expr".
    parse "1 + 2 + 3" with rule "expr '+' int | expr '-' int | int".
    parse "1 + 2 + 3" with rule "expr '+' int".    
    parse "1 + 2 + 3" with rule "expr".
    left recursion is recognized; set expr to FAIL; parse "1 + 2 + 3" with rule "expr".
    parse "1 + 2 + 3" with rule "FAIL(expr) '+' int | FAIL(expr) '-' int | int".
    parse "1 + 2 + 3" with rule "FAIL(expr) '+' int".    
    parse "1 + 2 + 3" with rule "FAIL(expr)".
    "expr" is in FAIL mode; return false.
    parse "1 + 2 + 3" with rule "FAIL(expr) '-' int | int".
    parse "1 + 2 + 3" with rule "FAIL(expr)".
    "expr" is in FAIL mode; return false.
    parse "1 + 2 + 3" with rule "int".
    "int" parses "1" successfully; remaining input = "+ 2 + 3".
    set expr to ACCEPT; parse "+ 2 + 3" with rule "expr".
    parse "+ 2 + 3" with rule "ACCEPT(expr) '+' int | ACCEPT(expr) '-' int | int".
    parse "+ 2 + 3" with rule "ACCEPT(expr) '+' int".
    parse "+ 2 + 3" with rule "ACCEPT(expr)".
    "expr" is in ACCEPT mode; return true.
    parse "+ 2 + 3" with rule "'+' int".
    '+' parses "+" successfully; parse "2 + 3" with rule "int".
    "int" parses "2" successfully; rule 'expr' is matched; remaining input = "+ 3".
    set expr to ACCEPT; parse "+ 3" with rule "expr".
    parse "+ 3" with rule "ACCEPT(expr) '+' int | ACCEPT(expr) '-' int | int".
    parse "+ 3" with rule "ACCEPT(expr) '+' int".
    parse "+ 3" with rule "ACCEPT(expr)".
    "expr" is in ACCEPT mode; return true.
    parse "+ 3" with rule "'+' int".
    '+' parses "+" successfully; parse "3" with rule "int".
    "int" parses "3" successfully; rule 'expr' is matched; remaining input = none; end.

The parse tree created from the above is:

   '+'
    |--'+'
    |   |---1
    |   |---2
    |---3
    
So, the algorithm preserves left associativity.


        
    
    
    
    
    
    
    