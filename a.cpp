#include <iostream>
#include <string>
#include <sstream>
#include <bitset>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>

using namespace std;

const unsigned ADD = 0; //000 rg1 rg2 rg3 0000
const unsigned SUB = 1; //000 rg1 rg2 rg3 0001
const unsigned AND = 2; //000 rg1 rg2 rg3 0010 
const unsigned OR = 3;  //000 rg1 rg2 rg3 0011 
const unsigned SLT = 4; //000 rg1 rg2 rg3 0100 
const unsigned JR = 8;  //000 rg1 000 000 1000 
const unsigned SLTI = 57344; //111 rg1 rg2 7b'imm
const unsigned LW = 32768;   //100 rg1 rg2 7b'imm
const unsigned SW = 40960; //101 rg1 rg2 7b'imm
const unsigned JEQ = 49152; //110 rg1 rg2 7b'rel_imm
const unsigned ADDI = 8192; //001 rg1 rg2 7b'imm
const unsigned J = 16384; //010 13b'imm
const unsigned JAL = 24576; //011 13b'imm

map<string, int> labels;

// enum class Opcode : unsigned {
//     ADD = 0, //000 rg1 rg2 rg3 0000
//     SUB = 1, //000 rg1 rg2 rg3 0001
//     AND = 2, //000 rg1 rg2 rg3 0010 
//     OR = 3,  //000 rg1 rg2 rg3 0011 
//     SLT = 4, //000 rg1 rg2 rg3 0100 
//     JR = 8,  //000 rg1 000 000 1000 
//     SLTI = 57344, //111 rg1 rg2 7b'imm
//     LW = 32768,   //100 rg1 rg2 7b'imm
//     SW = 40960, //101 rg1 rg2 7b'imm
//     JEQ = 49152, //110 rg1 rg2 7b'rel_imm
//     ADDI = 8192, //001 rg1 rg2 7b'imm
//     J = 16384, //010 13b'imm
//     JAL = 24576 //011 13b'imm
// };

ifstream filenameLoader(int argc, char* argv[], char*& filename){
    /*
        Parse the command-line arguments
    */
    //char* filename = nullptr;
    bool do_help = false;
    bool arg_error = false;
    for (int i = 1; i < argc; i++) {
        string arg(argv[i]);
        if (arg.rfind("-", 0) == 0) {
            if (arg == "-h" || arg == "--help")
                do_help = true;
            else
                arg_error = true;
        }
        else {
            if (filename == nullptr)
                filename = argv[i];
            else
                arg_error = true;
        }
    }
    /* Display error message if appropriate */
    if (arg_error || do_help || filename == nullptr) {
        cerr << "usage " << argv[0] << " [-h] filename" << endl << endl;
        cerr << "Assemble E20 files into machine code" << endl << endl;
        cerr << "positional arguments:" << endl;
        cerr << "  filename    The file containing assembly language, typically with .s suffix" << endl << endl;
        cerr << "optional arguments:" << endl;
        cerr << "  -h, --help  show this help message and exit" << endl;
        return ifstream();
    }

    /* iterate through the line in the file, construct a list
       of numeric values representing machine code */
    ifstream f(filename);
    if (!f.is_open()) {
        cerr << "Can't open file " << filename << endl;
        return ifstream();
    }
    return f;
}

void lineParser(ifstream& f, vector<string>& lines) {
    string line, label;
    while (getline(f, line)) {
        // Remove comments
        size_t commentPos = line.find("#");
        if (commentPos != string::npos) {
            line = line.substr(0, commentPos);
        }

        // Trim whitespace
        size_t begin = line.find_first_not_of(" \n\r\t\f\v");
        size_t end = line.find_last_not_of(" \n\r\t\f\v");
        line = (begin == string::npos) ? "" : line.substr(begin, (end == string::npos) ? string::npos : end - begin + 1);

        // Check for label
        size_t labelPos = line.find(':');
        if (labelPos != string::npos) {
            label = line.substr(0, labelPos + 1);
            if (labelPos + 1 < line.length()) {
                // Combine label with instruction on the same line
                lines.push_back(label + " " + line.substr(labelPos + 1));
                label.clear();
            }
        } else {
            if (!line.empty()) {
                if (!label.empty()) {
                    lines.push_back(label + " " + line);
                    label.clear();
                } else {
                    lines.push_back(line);
                }
            }
        }
    }
}

class Instruction {
public:
    Instruction(const string& line, int index) {
        this->index = index;
        parseInstruction(line);
    }

    void parseInstruction(const string& line) {
        size_t labelEnd = line.find(':');
        size_t start = 0;

        // Parse label if exists
        if (labelEnd != string::npos) {
            label = line.substr(0, labelEnd);
            removeWhitespace(label);
            labels[label] = index;
            start = labelEnd + 1; // Start looking for opcode after the label
        }

        // Find the start position of the opcode
        size_t opcodeStart = line.find_first_not_of(" \n\r\t\f\v", start);

        // If there's no opcode, return
        if (opcodeStart == string::npos) {
            return;
        }

        // Find the end of the opcode
        size_t spacePos = line.find(' ', opcodeStart);

        // Extract opcode
        if (spacePos == string::npos) {
            // The rest of the line is the opcode
            opcode = line.substr(opcodeStart);
        } else {
            // The opcode ends at the first space
            opcode = line.substr(opcodeStart, spacePos - opcodeStart);
        }

        // Parse operands if there are any
        if (spacePos != string::npos && spacePos + 1 < line.length()) {
            string operandsStr = line.substr(spacePos + 1);
            parseOperands(operandsStr);
        }
    }

    static void removeWhitespace(string& a) {
        size_t begin = a.find_first_not_of(" \n\r\t\f\v");
        a = (begin == string::npos) ? "" : a.substr(begin);
    }

    bool isNumeric(const string& str) {
        return !str.empty() && std::all_of(str.begin(), str.end(), [](char c) { return ::isdigit(c) || c == '-'; });
    }

    void parseOperands(const string& operandsStr) {
        stringstream ss(operandsStr);
        string operand;

        while (getline(ss, operand, ',')) {
            removeWhitespace(operand);

            if (operand.empty()) {
                continue;
            }

            if ((opcode == "lw" || opcode == "sw") && operand.find('(') != string::npos) {
                // Special handling for 'lw' and 'sw' instructions
                size_t openParen = operand.find('(');
                size_t closeParen = operand.find(')');

                string immOrLabel = operand.substr(0, openParen);
                string registerPart = operand.substr(openParen + 1, closeParen - openParen - 1);

                if (isNumeric(immOrLabel)) {
                    // Immediate value
                    operands.push_back(immOrLabel);
                } else if (labels.find(immOrLabel) != labels.end()) {
                    // Label value
                    operands.push_back(to_string(labels[immOrLabel]));
                } else {
                    // Error case: handle accordingly
                }

                if (!registerPart.empty() && registerPart[0] == '$') {
                    // Register part
                    operands.push_back(registerPart.substr(1));
                } else {
                    // Error case: handle accordingly
                }
            } else {
                // Handle other operands
                if (operand[0] == '$') {
                    operands.push_back(operand.substr(1)); // Extract register number
                } else if (isNumeric(operand)) {
                    operands.push_back(operand); // Direct immediate value
                } else if (labels.find(operand) != labels.end()) {
                    operands.push_back(to_string(labels[operand])); // Label value
                } else {
                    if (labels.find(operand) != labels.end()) {
                        operands.push_back(to_string(labels[operand])); // Label value
                    } else {
                        operands.push_back(to_string(labels.size() + 1)); // Default address for unresolved labels
                    }
                }
            }
        }
    }

    string getOpcode() const {
        return opcode;
    }

    vector<int> getOperandsAsInt() const {
        vector<int> intOperands;
        for (const auto& op : operands) {
            if (labels.find(op) != labels.end()) {
                // Operand is a label; use its associated value from the labels map
                intOperands.push_back(labels.at(op));
            } else {
                // Operand is not a label; convert string to int
                intOperands.push_back(stoi(op));
            }
        }
        return intOperands;
    }

    int getIndex() const {
        return index;
    }

    string toString() const {
        stringstream ss;
        if (!label.empty()) {
            ss << label << ": ";
        }
        ss << "opcode: " << opcode;
        for (const auto& operand : operands) {
            ss << " rg: " << operand;
        }
        return ss.str();
    }

private:
    string label;
    string opcode;
    vector<string> operands;
    int index;
    //static map<string, int> labels;
};

void firstPassForLabels(const vector<string>& lines) {
    int instructionIndex = 0;
    for (const auto& line : lines) {
        size_t labelPos = line.find(':');
        if (labelPos != string::npos) {
            string label = line.substr(0, labelPos);
            Instruction::removeWhitespace(label); // Assuming you have a static method for this
            labels[label] = instructionIndex;
        }
        instructionIndex++;
    }
}


unsigned full_address_parser(const Instruction& instruction) {
    string opcode = instruction.getOpcode(); // Assuming getOpcode() returns the string opcode
    vector<int> rgs = instruction.getOperandsAsInt(); // Assuming getOperandsAsInt() returns vector<int>
    int index = instruction.getIndex(); // Assuming getIndex() returns the instruction index

    if (opcode == "add") {
        return ADD | ((rgs[1] & 7) << 10) | ((rgs[2] & 7) << 7) | ((rgs[0] & 7) << 4);
    }
    if (opcode == "sub") {
        return SUB | ((rgs[1] & 7) << 10) | ((rgs[2] & 7) << 7) | ((rgs[0] & 7) << 4);
    }
    if (opcode == "and") {
        return AND | ((rgs[1] & 7) << 10) | ((rgs[2] & 7) << 7) | ((rgs[0] & 7) << 4);
    }
    if (opcode == "or") {
        return OR | ((rgs[1] & 7) << 10) | ((rgs[2] & 7) << 7) | ((rgs[0] & 7) << 4);
    }
    if (opcode == "slt") {
        return SLT | ((rgs[1] & 7) << 10) | ((rgs[2] & 7) << 7) | ((rgs[0] & 7) << 4);
    }
    if (opcode == "jr") {
        return JR | ((rgs[0] & 7) << 10);
    }
    //Instructions with two register arguments
    if (opcode == "slti") {
        return SLTI | ((rgs[1] & 7) << 10) | ((rgs[0] & 7) << 7) | (rgs[2] & 127);
    }
    if (opcode == "lw") {
        return LW | (rgs[2] & 7) << 10 | ((rgs[0] & 7) << 7) | (rgs[1] & 127);
    }
    if (opcode == "sw") {
        return SW | ((rgs[2] & 7) << 10) | ((rgs[0] & 7) << 7) | (rgs[1] & 127);
    }
    if (opcode == "jeq") {
        return JEQ | ((rgs[0] & 7) << 10) | ((rgs[1] & 7) << 7) | ((rgs[2] - index - 1) & 127);
    }
    if (opcode == "addi") {
        return ADDI | ((rgs[1] & 7) << 10) | ((rgs[0] & 7) << 7) | (rgs[2] & 127);
    }
    if (opcode == "j") {
        return J | (rgs[0] & 8191);
    }
    if (opcode == "jal") {
        return JAL | (rgs[0] & 8191);
    }
    //psuedo instructions
    if (opcode == "movi") {
        return ADDI | ((0) << 10) | ((rgs[0] & 7) << 7) | (rgs[1] & 127);
    }
    if (opcode == "nop") {
        return ADD | (((0) << 10) | ((0) << 7) | ((0) << 4));
    }
    if (opcode == "halt") {
        return J | ((index));
    }
    if (opcode == ".fill") {
        return rgs[0];
    }
    return -1; // unknown
}

vector<string> assemblerOutput(int argc, char* argv[]) {
    char* filename = nullptr;
    ifstream f = filenameLoader(argc, argv, filename);
    if (!f) return {};
    vector<string> lines;

    lineParser(f, lines);
    int counter = 0;
    // for (const auto& inst : lines) {
    //     cout << counter << ": "<< inst << endl;
    //     counter++;
    // }

    firstPassForLabels(lines);

    // for(const auto& tt: labels){
    //     cout << tt.first << ": " << tt.second << endl;
    // }
    vector<Instruction> instructions;

    for (int i = 0; i < lines.size(); ++i) {
        instructions.emplace_back(lines[i], i);
    }

    // // Print parsed instructions
    // for (const auto& inst : instructions) {
    //     cout << inst.toString() << endl;
    // }

    int program_counter = 0;

    vector<string> result;

    for (size_t i = 0; i < instructions.size(); ++i) {
        std::ostringstream oss;
        bitset<16> instruction_in_binary(full_address_parser(instructions[i]));
        //cout << "ram[" << program_counter << "] = 16'b" << instruction_in_binary << ";" << endl;
        oss << "ram[" << program_counter << "] = 16'b" << instruction_in_binary << ";";
        result.push_back(oss.str());
        program_counter++;
    }
    return result;
}


// int main(int argc, char* argv[]){
    
//     return 0;
// }