#include "cpp_build.hpp"
#include "parserlib.hpp"
using namespace cpp_build;


int main(int argc, char* argv[])
{
    CommandLine commandLine(argc, argv);

    if (commandLine.has("clean"))
    {
        std::cout << "parserlib: clean tests\n";
        std::filesystem::remove_all("release");
    }

    if (commandLine.has("build"))
    {
        std::cout << "parserlib: build tests\n";

        LibraryList libraries{parserlib()};
        FileList includeFolders("include");
        includeFolders.addIncludeFolders(libraries);
        includeFolders.add("..");
        FileList sourceFiles;
        sourceFiles.add("../examples/calculator/calculator_grammar.cpp");
        sourceFiles.add("calculator_tests.cpp");
        sourceFiles.add("../examples/xml/xml_grammar.cpp");
        sourceFiles.add("xml_tests.cpp");
        sourceFiles.add("parserlib_tests_main.cpp");
        
        compile("release/obj",
            {},
            includeFolders, 
            sourceFiles, 
            "g++ -c -O3 -std=c++17 -D NDEBUG { -I$includeFolders} -o {$objectFile} {$sourceFile}");

        link(FileList("release/obj", ".o"), 
            "release/bin/parserlib_tests.exe",
            {},
            {},
            "g++ -g -o {$target} { $objectFiles}");

        makeExecutable("release/bin/parserlib_tests.exe");
    }
    
    if (commandLine.has("run"))
    {
        execute("release/bin/parserlib_tests.exe");
    }
}
