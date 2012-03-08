#include <iostream>
#include "parserlib.hpp"
using namespace std;
using namespace parserlib;


extern rule exp, add, mul;


rule ws = *expr(' ');


rule digit = range('0', '9');


rule num = +digit >> -('.' >> +digit >> -(set("eE") >> -set("+-") >> +digit));


rule val = num
         | '(' >> exp >> ')';


rule mulOp = '*' >> mul;
rule divOp = '/' >> mul;
rule mul = val >> -(mulOp | divOp);


rule addOp = '+' >> add;
rule subOp = '-' >> add;
rule add = mul >> -(addOp | subOp);


rule exp = add;


void num_parse(const pos &b, const pos &e, void *d) {
    cout << "num: ";
    for(input::const_iterator it = b.m_it; it != e.m_it; ++it) {
        cout << (char)*it;
    }
    cout << endl;
}


void mul_parse(const pos &b, const pos &e, void *d) {
    cout << "mul: ";
    for(input::const_iterator it = b.m_it; it != e.m_it; ++it) {
        cout << (char)*it;
    }
    cout << endl;
}


void add_parse(const pos &b, const pos &e, void *d) {
    cout << "add: ";
    for(input::const_iterator it = b.m_it; it != e.m_it; ++it) {
        cout << (char)*it;
    }
    cout << endl;
}


void exp_parse(const pos &b, const pos &e, void *d) {
    cout << "exp: ";
    for(input::const_iterator it = b.m_it; it != e.m_it; ++it) {
        cout << (char)*it;
    }
    cout << endl;
}


static class setup {
public:
    setup() {
        num.set_parse_proc(num_parse);        
        mul.set_parse_proc(mul_parse);
        add.set_parse_proc(add_parse);
        exp.set_parse_proc(exp_parse);
    }
} setup;


//main
int main() {
    const char *src = "1 + (2 * 3)";
    input i(src, src + strlen(src));
    error_list el;
    bool ok = parse(i, exp, ::ws, el);
    if (ok) {
        cout << "success\n";
    }
    else {
        for(error_list::iterator it = el.begin();
            it != el.end();
            ++it)
        {
            error &err = *it;
            cout << "error at line " << err.m_begin.m_line << ", col " << err.m_begin.m_col << ": ";
            wcout << err.m_msg << endl;
        }
    }
    getchar();
    return 0;
}
