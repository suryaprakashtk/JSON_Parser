#include <iostream>
#include <fstream>
#include <functional>
#include <map>
#include "JSONParser.h"
#include "AutoGrader.h"
#include "Testable.h"
#include "Debug.h"

using namespace JSONParserNamespace;

inline std::string getWorkingDirectoryPath() {
    std::cout<<"Please change the path in your system"<<std::endl;
    return "Change your working directory here. Keep the full path";
}

bool runAutoGraderTest(const std::string& aPath, const std::string& aTestName) {
    AutoGrader autoGrader(aPath);
    return autoGrader.runTest(aTestName);
}

bool runNoFilterTest(const std::string& aPath) {
    return runAutoGraderTest(aPath, "NoFilterTest");
}

bool runBasicTest(const std::string& aPath) {
    return runAutoGraderTest(aPath, "BasicTest");
}

bool runAdvancedTest(const std::string& aPath) {
    return runAutoGraderTest(aPath, "AdvancedTest");
}

int runTest(const int argc, const char* argv[]) {
    const std::string thePath = argc > 2 ? argv[2] : getWorkingDirectoryPath();
    const std::string theTest = argv[1];

    std::map<std::string, std::function<bool(const std::string&)>> theTestFunctions {
        { "compile", [](const std::string&) { return true; } },
        { "nofilter", runNoFilterTest },
        { "query", runModelQueryTest },
        { "basic", runBasicTest },
        { "advanced", runAdvancedTest }
    };

    if (theTestFunctions.count(theTest) == 0) {
        std::clog << "Unkown test '" << theTest << "'\n";
        return 1;
    }

    const bool hasPassed = theTestFunctions[theTest](thePath);
    std::cout << "Test '" << theTest << "' " << (hasPassed ? "PASSED" : "FAILED") << "\n";
    return !hasPassed;
}

int main(const int argc, const char* argv[]) {
    
    if (argc > 1)
        runTest(argc, argv);

    // Put any JSON in Resources folder and it will be parsed by below code!
    std::fstream theJsonFile(getWorkingDirectoryPath() + "/Resources/sammy.json");
    JSONParser theParser(theJsonFile);
    Model theModel;
    theParser.parse(&theModel);

    // Query the model...
    auto theQuery = theModel.createQuery();
    auto theResult = theQuery.select("'sammy'").get("'username'");
    std::cout << theResult.value_or("No result found") << std::endl;
    return 0;
}