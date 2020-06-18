#include "cpp_build.hpp"
using namespace cpp_build;


int main(int argc, char* argv[])
{
    CommandLine commandLine(argc, argv);
    
    if (commandLine.has("clean"))
    {
        execute("tests/build.cpp", {"clean"});
        execute("examples/calculator/build.cpp", {"clean", "debug", "release"});
    }
    
    if (commandLine.has("install"))
    {
        Library library("parserlib");
        library.addIncludeFolder("include");
        installUserLibrary(library);
    }
    
    if (commandLine.has("run_tests"))
    {
        execute("tests/build.cpp", {"build", "run"});
    }

    if (commandLine.has("build_examples"))
    {
        execute("examples/calculator/build.cpp", {"build", "debug", "release"});
    }
}
