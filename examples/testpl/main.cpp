#include "parserlib/util.hpp"
#include "parser.hpp"


//load and compile a file
int main() {
    std::vector<Error> errors;

    //load source
    std::string file = parserlib::loadASCIIFile("../examples/test1.pl");

    //tokenize
    const std::vector<Token> tokens = tokenize(file, errors);

    //parse
    const std::vector<AST> ast = parse(tokens, errors);

    return 0;
}
