# How left recursion is handled

## the problem

Suppose we have the following grammar:

	rule mul = mul >> '*' >> num
             | mul >> '-' >> num
             | num;

	rule add = add >> '+' >> mul
             | add >> '-' >> mul
             | mul;

If there was no left recursion handling, the rule `add` would be rentered in a continuous loop, and the program would not terminate: the rule `add` would invoke the rule reference `add`, which would invoke the rule `add` etc.

## the solution

Parserlib handles left recursion in a very 'natural' way: it tries to avoid left-recursive branches.

If a left recursive branch is found, it is ignored, and parsing goes to the next available branch.

If a branch sucessfully parses, then the previous left-recursive branches are then invoked, with special instructions to avoid reentering left-recursive rule references.

For example, in the above calculator grammar, the parsing steps for the expression `1+2` would be as follows:

	1. enter branch 'add + mul'
	2. enter 'add'
	3. enter 'add'; left recursion is found; go to the next branch
	4. enter branch 'add - mul'
	5. enter 'add'
	6. enter 'add'; left recursion is found; go to the next branch
	7. enter 'mul'
	8. enter num
	9. parse '1'; success
	10. enter branch '- mul'
	11. enter '-'; failed; enter another branch
	12. enter branch '+ mul'
	13. parse '+'; success
	14. enter 'mul'
	15. enter 'num'
	16. parse '2'; success

