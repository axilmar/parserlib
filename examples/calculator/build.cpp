#include "cpp_build.hpp"
#include "parserlib.hpp"
using namespace cpp_build;


int main(int argc, char* argv[])
{
    CommandLine commandLine(argc, argv);

    bool debug = commandLine.has("debug");
    bool release = commandLine.has("release");

    if (commandLine.has("clean"))
    {        
        if (debug)
        {
            std::cout << "program1: clean debug\n";
            std::filesystem::remove_all("debug");
        }

        if (release)
        {
            std::cout << "program1: clean release\n";
            std::filesystem::remove_all("release");
        }
    }

    if (commandLine.has("build"))
    {
        LibraryList libraries{parserlib()};
        FileList includeFolders("include");
        includeFolders.addIncludeFolders(libraries);
        FileList sourceFiles;
        sourceFiles.add("calculator_grammar.cpp");
        sourceFiles.add("calculator_main.cpp");

        if (debug)
        {
            std::cout << "calculator: build debug\n";

            compile("debug/obj",
                    PreprocessorDefinitionList().addLibraries("debug", libraries),
                    includeFolders, 
                    sourceFiles, 
                    "g++ -c -O0 -std=c++17 -D DEBUG{ -D$preprocessorDefinitions}{ -I$includeFolders} -o {$objectFile} {$sourceFile}");

            link(FileList("debug/obj", ".o"), 
                 getExecutable("debug/bin/calculatord"),
                {},
                {},
                 "g++ -g -o {$target} { $objectFiles}");

            makeExecutable("debug/bin/calculatord");
        }

        if (release)
        {
            std::cout << "calculator: build release\n";

            compile("release/obj",
                    PreprocessorDefinitionList().addLibraries("release", libraries),
                    includeFolders, 
                    sourceFiles, 
                    "g++ -c -O3 -std=c++17 -D NDEBUG{ -D$preprocessorDefinitions} { -I$includeFolders} -o {$objectFile} {$sourceFile}");

            link(FileList("release/obj", ".o"), 
                 getExecutable("release/bin/calculator"),
                {},
                {},
                 "g++ -g -o {$target} { $objectFiles}");

            makeExecutable("release/bin/calculator");
        }
    }

    return 0;
}
