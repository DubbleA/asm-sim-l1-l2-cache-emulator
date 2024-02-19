#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "simtemp.cpp"

struct TestCase {
    std::string filename;
    std::string expected;
};

void testSimulator(){
    std::vector<TestCase> tests = {
        {"asmfiles/fib_iter.bin", 
R"(Final state:
 	pc=    7
 	$0=    0
 	$1=    6
 	$2=    0
 	$3=34788
 	$4=    0
 	$5=    0
 	$6=    0
 	$7=    0
2080 2180 8508 0d30 2481 c801 4002 4007 
01b2 0926 7cb4 0003 0055 0000 0000 0000 
0000 0000 0000 0000 0000 0000 0000 0000 
0000 0000 0000 0000 0000 0000 0000 0000 
0000 0000 0000 0000 0000 0000 0000 0000 
0000 0000 0000 0000 0000 0000 0000 0000 
0000 0000 0000 0000 0000 0000 0000 0000 
0000 0000 0000 0000 0000 0000 0000 0000 
0000 0000 0000 0000 0000 0000 0000 0000 
0000 0000 0000 0000 0000 0000 0000 0000 
0000 0000 0000 0000 0000 0000 0000 0000 
0000 0000 0000 0000 0000 0000 0000 0000 
0000 0000 0000 0000 0000 0000 0000 0000 
0000 0000 0000 0000 0000 0000 0000 0000 
0000 0000 0000 0000 0000 0000 0000 0000 
0000 0000 0000 0000 0000 0000 0000 0000)"
        },
    };

    for (const auto& test : tests) {
        // Prepare arguments for assemblerOutput
        const char* args[] = {"sim", test.filename.c_str()};
        string output = mainTemp(2, const_cast<char**>(args));
        
        bool testPassed = output == test.expected;

        std::cout << "Test " << test.filename << ": " << (testPassed ? "PASS" : "FAIL") << std::endl;
        if (!testPassed) {
            std::cout << "Expected:" << std::endl;
            cout << test.expected << endl;
            std::cout << "Got:" << std::endl;
            cout << output << endl;
        }
    }
}

int main(){
    testSimulator();
    return 0;
}

//g++ -std=c++20 -Isrc sim_test.cpp -o sim_test && ./sim_test