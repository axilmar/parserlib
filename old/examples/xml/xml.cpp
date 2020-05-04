#include <stdio.h>
#include <iostream>
#include <string>
#include "parserlib.hpp"
using namespace std;
using namespace parserlib;


/******************************************************************************
    GRAMMAR
 ******************************************************************************/


extern rule node;


/**** TERMINALS ****/


//newline
#define NEWLINE              nl(expr("\r\n") | "\n\r" | '\n' | '\r')


//letter
#define LETTER               (range('a', 'z') | range('A', 'Z'))


//digit
#define DIGIT                range('0', '9')


//string character
#define STRING_CHAR          (!expr('"') >> range(0, 255))


//whitespace
rule whitespace = *(NEWLINE | range(0, 32));


//string
rule str = term('"' >> *STRING_CHAR >> '"');


//identifier
rule id = term(LETTER >> *(LETTER | DIGIT));


/**** NON-TERMINALS ****/


//attribute name
rule attr_name = id;


//attribute value
rule attr_value = str;


//attribute
rule attr = attr_name >> '=' >> attr_value;


//begin tag
rule begin_tag = id;


//end tag.
rule end_tag = id;


//node
rule node = '<' >> begin_tag >> *attr >> ('>' >> *node >> "</" >> end_tag >> '>' | "/>");


/******************************************************************************
    AST
 ******************************************************************************/


//identifier
class XMLId : public ast_node {
public:
    //value
    string m_value;

    //construct
    void construct(ast_stack &st) {
        m_value.insert(m_value.end(), m_begin.m_it, m_end.m_it);
    }
};


//string
class XMLString : public ast_node {
public:
    //value
    string m_value;

    //construct
    void construct(ast_stack &st) {
        m_value.insert(m_value.end(), m_begin.m_it + 1, m_end.m_it - 1);
    }
};


//attribute name
class XMLAttrName : public XMLId {};


//attribute value
class XMLAttrValue : public XMLString {};


//attribute
class XMLAttr : public ast_container {
public:
    //name
    ast_ptr<XMLAttrName> name;

    //value
    ast_ptr<XMLAttrValue> value;

    //print the attribute
    void print() {
        cout << name->m_value << " = \"" << value->m_value << "\"";
    }
};


//begin tag
class XMLBeginTag : public XMLId {};


//end tag
class XMLEndTag : public XMLId {};


//XML node
class XMLNode : public ast_container {
public:
    //begin tag
    ast_ptr<XMLBeginTag> beginTag;

    //attributes
    ast_list<XMLAttr> attrs;

    //children nodes
    ast_list<XMLNode> children;

    //end tag
    ast_ptr<XMLEndTag, true> endTag;

    //print
    void print(int ident) {
        //print the begin tag
        for(int i = 0; i < ident; ++i) cout << ' ';
        cout << "<" << beginTag->m_value;

        //print attributes
        for(ast_list<XMLAttr>::container::const_iterator it = attrs.objects().begin();
            it != attrs.objects().end();
            ++it)
        {
            cout << ' ';
            (*it)->print();
        }

        //print end of node without children
        if (children.objects().empty()) {
            cout << "/>\n";
        }

        //print node with children
        else {
            cout << ">\n";

            //print children nodes
            for(ast_list<XMLNode>::container::const_iterator it = children.objects().begin();
                it != children.objects().end();
                ++it)
            {
                (*it)->print(ident + 4);
            }

            //print the end tag
            for(int i = 0; i < ident; ++i) cout << ' ';
            cout << "</" << beginTag->m_value << ">\n";
        }
    }
};


/******************************************************************************
    GRAMMAR-AST
 ******************************************************************************/


ast<XMLAttrName> attr_name_ast(attr_name);
ast<XMLAttrValue> attr_value_ast(attr_value);
ast<XMLAttr> attr_ast(attr);
ast<XMLBeginTag> begin_tag_ast(begin_tag);
ast<XMLEndTag> end_tag_ast(end_tag);
ast<XMLNode> node_ast(node);


/******************************************************************************
    MAIN
 ******************************************************************************/


//load a text file into memory
static bool load_text_file(const char *filename, input &i) {
    //open file
    FILE *file = fopen(filename, "rt");
    if (!file) return false;

    //find its size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    //create a buffer to hold the whole file
    vector<unsigned char> buf(size);

    //load it
    if (size > 0) {
        fread(&buf[0], 1, size, file);
    }

    //test for error
    bool ok = ferror(file) == 0;

    //close the file
    fclose(file);

    //check for error
    if (!ok) return false;

    //convert the file to the input
    i.insert(i.end(), buf.begin(), buf.end());

    //success
    return true;
}


//main
int main(int argc, char *argv[]) {
    //check the parameters
    if (argc < 2) {
        cout << "ERROR: xml file not specified\n";
        return 0;
    }

    //load the file
    input source;
    if (!load_text_file(argv[1], source)) {
        cout << "ERROR: xml file could not be loaded\n";
        return 0;
    }

    //parse the file
    error_list errors;
    XMLNode *root;
    parse(source, node, whitespace, errors, root);

    //sort and print errors
    if (errors.empty()) {
        cout << "no errors found\n";
        if (root) root->print(0);
    }
    else {
        cout << "found " << errors.size() << " " << (errors.size() > 1 ? "errors" : "error") << ":\n";
        errors.sort();
        for(error_list::iterator it = errors.begin();
            it != errors.end();
            ++it)
        {
            error &e = *it;
            cout << "    line "
                 << e.m_begin.m_line
                 << ", col "
                 << e.m_begin.m_col
                 << ": ";
            //wcout << e.m_msg;
            wcout << endl;
        }
    }

    cout << "press any key...\n";
    getchar();
    return 0;
}
