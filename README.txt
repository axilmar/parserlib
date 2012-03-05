Introduction
------------

ParserLib is a simple c++ PEG parser. Its aim is to be very simple in use.

Grammars
--------

Grammars can be written in pseudo-EBNF form, as in Boost::Spirit, like this:

    //calculator grammar.
    extern rule expression;
    rule whitespace = *term(' ');
    rule digit = set("0123456789");
    rule integer = term(+digit);
    rule num_expression = integer | '(' >> expression >> ')';
    rule mul_expression = num_expression >> ('*' >> add_expression | '/' >> add_expression);
    rule add_expression = mul_expression >> ('+' >> add_expression | '-' >> add_expression);
    rule expression = add_expression;

Parsing
-------

Parsing can then be achieved by this piece of code:

    ErrorList errors;
    AST *ast = parse("1 + (2 * 3)", expression, whitespace, errors);

AST
---

The result of the above code, should there be no errors, should be an AST instance. The AST instance itself is linked with a rule, and has children. Each parsed element in the grammar gets an AST node. There are two kinds of AST nodes, non-terminals and terminals. Processing an AST tree can then be as simple as the following function:

    void process_AST(AST *ast) {
        NonTermAST *nonTerm;

        switch (ast->type()) {
            case NON_TERMINAL:
                if (ast->is_rule(num_expression)) {
                    print("integer found\n");
                }
                else if (ast->is_rule(mul_expression)) {
                    print("multiplication found\n");
                }
                else if (ast->is_rule(add_expression)) {
                    print("addition found\n");
                }
                nonTerm = (NonTermAST *)ast;
                for(ASTContainer::const_iterator it = nonTerm->children().begin();
                    it != nonTerm->children().end();
                    ++it)
                {
                    process_AST(*it);
                }
                break;

            case TERMINAL:
                print_terminal((TermAST *)ast);
                break;
        }
    }

Errors
------

Errors can be processed like this:

    for(ErrorList::iterator it = errors.begin(); it != errors.end(); ++it) {
        Error &error = *it;
        cout << "error at line " << error.line() << ", column " << error.column() << ": ";
        switch (error.type()) {
            case LEXER_ERROR:
                cout << "invalid characters\n";
                break;
            case PARSER_ERROR:
                cout << "syntax error\n";
                break;
        }
        cout << endl;
    }


Why not Boost::Spirit?
----------------------

Two reasons:

    * I find Boost::Spirit extremely complicated for my taste.
    * all the template meta-programming makes compiling slow.

How is whitespace parsed?
-------------------------

Whitespace parsing is done in-between each terminal declared in the grammar. When the parser finds a terminal, it sets a flag to stop parsing whitespace within the terminal, and thus terminals can contain whitespace characters. The flag is reset after the terminal parsing ends.

Usage
-----

Just include "parserlib.hpp" and "parserlib.cpp" in your project.
The namespace is "parserlib" (of course).
