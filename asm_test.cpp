#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "a.cpp"

struct TestCase {
    std::string filename;
    std::vector<std::string> expected;
};
void assembleInstructions(const std::string& filename, std::vector<std::string>& output);

void testAssembler(){
    std::vector<TestCase> tests = {
        {"asmfiles/fib_iter.s", {
            "ram[0] = 16'b0010000010001000;",
            "ram[1] = 16'b0010000100000000;",
            "ram[2] = 16'b0010000110000001;",
            "ram[3] = 16'b1100010000000101;",
            "ram[4] = 16'b0010010011111111;",
            "ram[5] = 16'b0000100111000000;",
            "ram[6] = 16'b0000000110100000;",
            "ram[7] = 16'b0000001000110000;",
            "ram[8] = 16'b0100000000000011;",
            "ram[9] = 16'b0000000110010000;",
            "ram[10] = 16'b0100000000001010;"
        }},
        {"asmfiles/array-sum.s", {
            "ram[0] = 16'b0010000010000000;",
            "ram[1] = 16'b0010000110000000;",
            "ram[2] = 16'b1000010100001000;",
            "ram[3] = 16'b0000110100110000;",
            "ram[4] = 16'b0010010010000001;",
            "ram[5] = 16'b1100100000000001;",
            "ram[6] = 16'b0100000000000010;",
            "ram[7] = 16'b0100000000000111;",
            "ram[8] = 16'b0000000110110010;",
            "ram[9] = 16'b0000100100100110;",
            "ram[10] = 16'b0111110010110100;",
            "ram[11] = 16'b0000000000000011;",
            "ram[12] = 16'b0000000001010101;",
            "ram[13] = 16'b0000000000000000;"
        }},
        {"asmfiles/loop1.s", {
            "ram[0] = 16'b0010000010001010;",
            "ram[1] = 16'b1100010000000010;",
            "ram[2] = 16'b0010010011111111;",
            "ram[3] = 16'b0100000000000001;",
            "ram[4] = 16'b0100000000000100;"
        }},
        {"asmfiles/loop2.s", {
            "ram[0] = 16'b0010000010001010;",
            "ram[1] = 16'b0100000000000011;",
            "ram[2] = 16'b0100000000000010;",
            "ram[3] = 16'b1100010001111110;",
            "ram[4] = 16'b0010010011111111;",
            "ram[5] = 16'b0100000000000011;"
        }},
        {"asmfiles/loop3.s", {
            "ram[0] = 16'b0010000010111111;",
            "ram[1] = 16'b0010001100000001;",
            "ram[2] = 16'b0100000000000100;",
            "ram[3] = 16'b0100000000000011;",
            "ram[4] = 16'b1110011010001010;",
            "ram[5] = 16'b1101011101111101;",
            "ram[6] = 16'b0010010011111101;",
            "ram[7] = 16'b0100000000000100;"
        }},
        {"asmfiles/math.s", {
            "ram[0] = 16'b0010000010000101;",
            "ram[1] = 16'b0010010101111110;",
            "ram[2] = 16'b0000010100110000;",
            "ram[3] = 16'b0010001000110111;",
            "ram[4] = 16'b0001000011010001;",
            "ram[5] = 16'b0001011001000001;",
            "ram[6] = 16'b0000101011100011;",
            "ram[7] = 16'b0000101011110010;",
            "ram[8] = 16'b0100000000001000;"
        }},
        {"asmfiles/subroutine1.s", {
            "ram[0] = 16'b0010000010000001;",
            "ram[1] = 16'b0110000000000100;",
            "ram[2] = 16'b0010000100000010;",
            "ram[3] = 16'b0100000000000011;",
            "ram[4] = 16'b0010000110000011;",
            "ram[5] = 16'b0001110000001000;"
        }},
        {"asmfiles/subroutine2.s", {
            "ram[0] = 16'b0010000010000011;",
            "ram[1] = 16'b0110000000000111;",
            "ram[2] = 16'b0000000010100000;",
            "ram[3] = 16'b0010000010001001;",
            "ram[4] = 16'b0110000000000111;",
            "ram[5] = 16'b0000000010110000;",
            "ram[6] = 16'b0100000000000110;",
            "ram[7] = 16'b0000010010010000;",
            "ram[8] = 16'b0000010010010000;",
            "ram[9] = 16'b0001110000001000;"
        }},
        {"asmfiles/vars1.s", {
            "ram[0] = 16'b1000000010001001;",
            "ram[1] = 16'b1000000100001010;",
            "ram[2] = 16'b0000010100110010;",
            "ram[3] = 16'b0000010101000011;",
            "ram[4] = 16'b1010000110001011;",
            "ram[5] = 16'b1000001110001011;",
            "ram[6] = 16'b0010001010001011;",
            "ram[7] = 16'b0011011100001100;",
            "ram[8] = 16'b0100000000001000;",
            "ram[9] = 16'b0000000000011110;",
            "ram[10] = 16'b0000000000000101;",
            "ram[11] = 16'b0000000000000000;"
        }}
    };

    for (const auto& test : tests) {
        // Prepare arguments for assemblerOutput
        const char* args[] = {"asm", test.filename.c_str()};
        std::vector<std::string> output = assemblerOutput(2, const_cast<char**>(args));
        
        bool testPassed = true;

        if (output.size() != test.expected.size()) {
            testPassed = false;
        } else {
            for (size_t i = 0; i < output.size(); i++) {
                if (output[i].compare(test.expected[i]) != 0) {
                    testPassed = false;
                    break;
                }
            }
        }

        std::cout << "Test " << test.filename << ": " << (testPassed ? "PASS" : "FAIL") << std::endl;
        if (!testPassed) {
            std::cout << "Expected:" << std::endl;
            for (const auto& line : test.expected) {
                std::cout << line << std::endl;
            }
            std::cout << "Got:" << std::endl;
            for (const auto& line : output) {
                std::cout << line << std::endl;
            }
        }
    }
}

int main(){
    testAssembler();
    return 0;
}

//g++ -std=c++20 -Isrc asm_test.cpp -o asm_test && ./asm_test