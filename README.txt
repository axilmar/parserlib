Introduction
------------

ParserLib is a c++ recursive-descent PEG parser.

It can handle left recursion, as shown in the example below.

Version
-------

0.0.0.1.

Writing Grammars
----------------

Its purpose is to be very simple to use.

Grammars can be written in EBNF style, using c++ operators, ala boost::spirit.

The operators are:

    operator >>: sequence.
    operator | : choice.
    operator * : zero or more times loop.
    operator + : one or more times loop.
    operator - : optional.
    operator & : logical AND.
    operator ! : logical NOT.
    
The class 'rule' represents a grammar rule.    

For example, a calculator grammar can be written like this:

    extern rule exp, add, mul;

    //whitespace    
    rule ws = *expr(' ');

    //digit    
    rule digit = range('0', '9');

    //number    
    rule num = +digit >> -('.' >> +digit >> -(set("eE") >> -set("+-") >> +digit));

    //value    
    rule val = num
             | '(' >> exp >> ')';

    //multiplication/division
    rule mul_op = mul >> '*' >> val;
    rule div_op = mul >> '/' >> val;
    rule mul = mul_op 
             | div_op
             | val;

    //addition/subtraction
    rule add_op = add >> '+' >> mul;
    rule sub_op = add >> '-' >> mul;
    rule add = add_op
             | sub_op
             | mul;

    //expression
    rule exp = add;

For more information on PEG parsing, see this: 
    http://en.wikipedia.org/wiki/Parsing_expression_grammar    
    
Writing ASTs
------------

Abstract Syntax Trees can be written by extending the available ParserLib classes:

    class ast_node: base class for AST nodes.
    class ast_container: base class for AST nodes that have members.
    class ast_member : base class for classes that are static members of AST nodes.
    class ast_ptr<T>: AST member that points to an object of type T.
    class ast_list<T>: AST member that contains a list of T objects.

For example, writing an AST for the calculator above is very easy:

    //base class for expressions
    class expr_t : public ast_container {
    public:
        virtual double eval() const = 0;
    };

    //number
    class num_t : public expr_t {
    public:
        virtual void construct(const pos &b, const pos &e, ast_stack &st) {
            stringstream stream;
            for(input::iterator it = b.m_it; it != e.m_it; ++it) {
                stream << (char)*it;
            }
            stream >> m_value;
        }

        virtual double eval() const {
            return m_value;
        }

    private:
        double m_value;
    };

    //base class for binary expressions
    class binary_expr_t : public expr_t {
    public:
        ast_ptr<expr_t> left, right;
    };

    //addition
    class add_t : public binary_expr_t {
    public:
        virtual double eval() const {
            return left->eval() + right->eval();
        }
    };

    //subtraction
    class sub_t : public binary_expr_t {
    public:
        virtual double eval() const {
            return left->eval() - right->eval();
        }
    };

    //multiplication
    class mul_t : public binary_expr_t {
    public:
        virtual double eval() const {
            return left->eval() * right->eval();
        }
    };

    //division
    class div_t_ : public binary_expr_t {
    public:
        virtual double eval() const {
            return left->eval() / right->eval();
        }
    };
    
Connecting Grammars and ASTs.
-----------------------------

The above AST, although simple and consise, is not connected to the grammar.
In order to connect it to the grammar, we need to use the class ast<T>, which
connects rules to classes. 

For example, for the calculator:

    ast<num_t> ast_num(num);
    ast<add_t> ast_add(add_op);
    ast<sub_t> ast_sub(sub_op);
    ast<mul_t> ast_mul(mul_op);
    ast<div_t_> ast_div(div_op);
    
This is one of the major differences from boost::spirit, in which 
parser objects are embedded into the grammar. By connecting the grammar
and the AST outside of the grammar, the grammar remains clean and more readable.

Parsing
-------

The actual parsing is done by the 'parse' function, like this:

    //parse
    error_list el;
    expr_t *r;
    parse(i, exp, ::ws, el, r);
    
If the parsing function returns a non-null pointer to an AST node, then parsing was successful.
Otherwise, there was an error, and the error list is filled with error descriptions.

Other Examples
--------------

For more examples, please look at the folder 'examples'.

TODO
----

-more examples.
-full code documentation.
-account for tab size.
