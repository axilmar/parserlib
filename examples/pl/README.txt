PL is a test programming language implemented with parserlib.

It is not a regular programming language; it is more of a translator to c++:
the input program is parsed and then translated to c++.
It is sufficient to demonstrate how to declare ASTs with parserlib.

The file 'program.txt' contains a demo program with all the language constructs
supported by the language.

'Compiling' a program with the PL compiler results in a c++ program
in the same directory as the original source. Then, the c++ program
can compiled with any c++ compiler.

