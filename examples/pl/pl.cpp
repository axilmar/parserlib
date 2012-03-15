#include <cstdio>
#include <iostream>
#include "pl_grammar.hpp"
#include "pl_ast.hpp"
using namespace std;
using namespace parserlib;


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


//print help
void print_help() {
    cout << "PL compiler 1.0. Syntax:\n";
    cout << "pl <source file name>\n";
}


//main
int main(int argc, char *argv[]) {
    //if no file is specified, print the help
    if (argc < 2) {
        print_help();
        return 0;
    }

    //load the file
    input i;
    if (!load_text_file(argv[1], i)) {
        cout << "ERROR: file " << argv[1] << " could not be loaded\n";
        return 0;
    }

    cout << "compiling file " << argv[1] << "..." << endl;

    //parse the file
    error_list errors;
    ast_translation_unit *ast;
    parse(i, translation_unit, whitespace, errors, ast);

    //process ast tree
    if (ast) {
        ast->type_check(errors);
        delete ast;
    }

    //else sort and print errors
    if (errors.empty()) {
        cout << "no errors found\n";
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
            wcout << e.m_msg;
            wcout << endl;
        }
    }

    cout << "finished. Press any key to continue.\n";
    getchar();
    return 0;
}

