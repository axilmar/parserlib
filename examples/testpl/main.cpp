#include "parserlib/util.hpp"
#include "tokenizer.hpp"


//load and compile a file
int main() {
    std::vector<Error> errors;

    //load source
    std::string file = parserlib::loadASCIIFile("../examples/test1.pl");

    //tokenize
    std::vector<Token> tokens = tokenize(file, errors);

    return 0;
}
