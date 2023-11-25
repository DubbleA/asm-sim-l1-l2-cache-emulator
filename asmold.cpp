#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>

#include <cstring>
#include <algorithm>

#include <cstddef>
#include <bitset>


using namespace std;

unsigned ADD = 0; //000 rg1 rg2 rg3 0000
unsigned SUB = 1; //000 rg1 rg2 rg3 0001
unsigned AND = 2; //000 rg1 rg2 rg3 0010 
unsigned OR = 3;  //000 rg1 rg2 rg3 0011 
unsigned SLT = 4; //000 rg1 rg2 rg3 0100 
unsigned JR = 8;  //000 rg1 000 000 1000 
unsigned SLTI = 57344; //111 rg1 rg2 7b'imm
unsigned LW = 32768;   //100 rg1 rg2 7b'imm
unsigned SW = 40960; //101 rg1 rg2 7b'imm
unsigned JEQ = 49152; //110 rg1 rg2 7b'rel_imm
unsigned ADDI = 8192; //001 rg1 rg2 7b'imm
unsigned J = 16384; //010 13b'imm
unsigned JAL = 24576; //011 13b'imm


//The movi $reg, imm instruction is translated by the assembler as addi $reg, $0, imm.
//The nop instruction is translated by the assembler as add $0, $0, $0.
//The halt instruction is translated by the assembler as an unconditional jump (j) to the current memory location.
//.fill some_label Example: .fill 42 Inserts a 16-bit immediate value


/*
• All functions should have a documenting comment in the appropriate style describing its purpose,
behavior, inputs, and outputs. In addition, where appropriate, code should be commented to describe
its purpose and method of operation.
*/


//takes a vector of string references that will be changed respectfully.
//removes whitespace and if there is a label, it moves the next instruction to the same line
void instruction_parser(vector<string>&);

//takes in a string by reference
//removes extra whitespace from the string i.e. tabs // new lines
void remove_whitespace(string&);

//Label struct stores name and int value of label
struct Label {
    int value;
    string name;
    bool valid = false;
};
//takes in a string instruction after redundant whitespace has been removed
//if the instruction is not a label (calculated by detecting :) return what is before the first delimiter (space)
//else if there is a label then remove whitespace after the colon and extract the opcode from the leftovers
string opcode_delimiter(const string& instruction) {
    string opcode;
    if (instruction.find(':') == string::npos) {
        opcode = instruction.substr(0, instruction.find(' '));
    }
    else {
        string temp = instruction.substr(instruction.find(':') + 1);
        remove_whitespace(temp);
        opcode = temp.substr(0, temp.find(' '));
    }
    return opcode;
}

//does the same thing as the opcode delimiter instead we're extracting the second half instead of the first
//takes in a string instruction after redundant whitespace has been removed
//if the instruction is not a label (calculated by detecting :) return what is after the first delimiter (space)
//else if there is a label then remove whitespace after the colon and extract the address from the leftovers
//accounting for .fill
string address_delimiter(const string& instruction) {
    string address;
    if (instruction.find(':') == string::npos) {
        address = instruction.substr(instruction.find(' ') + 1);
        remove_whitespace(address);
        address.erase(remove(address.begin(), address.end(), ' '), address.end());

    }
    else if (instruction.find(':') != string::npos) {
        string temp = instruction.substr(instruction.find(':') + 1);
        remove_whitespace(temp);
        if (temp.find('.') != string::npos) {
            address = temp.substr(temp.find(' ') + 1);
            return address;
        }
        address = temp.substr(temp.find(' ') + 1);
        address.erase(remove(address.begin(), address.end(), ' '), address.end());
        return address;
    }
    return address;
}

//given a string and index if there is a colon make a label struct and make sure its valid.
//returns label struct
Label label_delimiter(const string& instruction, int index) {
    string label;
    Label labelStruct;
    if (instruction.find(':') != string::npos) {
        label = instruction.substr(0, instruction.find(':'));
        remove_whitespace(label);
        labelStruct.name = label;
        labelStruct.value = index;
        labelStruct.valid = true;
    }
    return labelStruct;
}

//if a string contains a number return true else return false
//takes in a partition of the string that it is trying to calculate for.
bool containsNum(const string& stringPartition)
{
    return std::find_if(stringPartition.begin(), stringPartition.end(), ::isdigit) != stringPartition.end();
}

//takes in a partition of the string, and the vector of labels to test against. 
//goal of this is to take our string and return a corresponding number value for our machine code building.
//if the string partition is a value, return the value of the label
//else if there is no number in the partition return 0
//if there is a $ designating a register, return the value of the register converted from string
//else if there is a number in the string return the conversion of the string from string to int
//else return 0 if none of the conditions have been hit.

unsigned rgs_parser(const string& partition, const vector<Label>& labels) {


    for (Label b : labels) {
        if (partition == b.name)
            return b.value;
    }

    if (!containsNum(partition)) {
        return 0;
    }

    if (partition.find('$') != string::npos) {
        return stoi(partition.substr(partition.find('$') + 1));
    }
    if (containsNum(partition))
        return stoi(partition);


    return 0;

}

//function takes the instruction part of the E20 Language line, and the vector of labels
//first goal of the program is to convert the instruction into a vector of strings that are split by the delimiter (,)
//we take that vector of strings and use the rgs parser to get the respective number value for the string
//returns a vector of ints. 
//In the case that there are only 1 or 2 register arguments, there will be duplicates in the 
//respective second and third index(s) which wont be accessed when building the machine code later. 

vector<int> three_comma_delimiter(const string& address, const vector<Label>& labels) {
    vector<string> rgs;
    string temp = address;
    vector<int> int_rgs;
    string substring;

    for (size_t i = 0; i < 3; i++) {

        substring = temp.substr(0, temp.find(','));
        remove_whitespace(substring);
        rgs.push_back(substring);
        temp = temp.substr(temp.find(',') + 1);
    }

    if (rgs[1].find('(') != string::npos) {
        int_rgs.push_back(rgs_parser(rgs[0], labels));
        string memoryImm = rgs[1].substr(0, rgs[1].find('('));
        //cout << memoryImm;
        int_rgs.push_back(rgs_parser(memoryImm, labels));
        memoryImm = rgs[1].substr(rgs[1].find('(') + 1);
        string registerVal = memoryImm.substr(0, memoryImm.find(')'));
        int_rgs.push_back(rgs_parser(registerVal, labels));
    }
    else {
        int_rgs.push_back(rgs_parser(rgs[0], labels));
        int_rgs.push_back(rgs_parser(rgs[1], labels));
        int_rgs.push_back(rgs_parser(rgs[2], labels));
    }

    for (size_t i = 0; i < int_rgs.size(); i++) {
        if (int_rgs[i] == 32766)
            int_rgs.erase(int_rgs.begin() + i);
    }

    return int_rgs;
}

/*
constant values of the integer number using the Decimal to binary converter (which was nice
for organization and debugging) to create the basis for my machine code building.
Then by utilizing the bitwise operations outlined in the hint section of the project
( & and << ) I manually built the machine code based on the opcode in a series of if statements.

takes in the opcode, index (program counter), and the vector of ints parsed from the instruction component
returns the unsigned integer value that will be converted into the 16 bit machine code
*/

unsigned full_address_parser(const string& opcode, int index, const vector<int>& rgs) {
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
}

/**
    Main function
    Takes command-line args as documented below
*/
int main(int argc, char* argv[]) {
    /*
        Parse the command-line arguments
    */
    char* filename = nullptr;
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
        return 1;
    }

    /* iterate through the line in the file, construct a list
       of numeric values representing machine code */
    ifstream f(filename);
    if (!f.is_open()) {
        cerr << "Can't open file " << filename << endl;
        return 1;
    }

    /* our final output is a list of ints values representing
       machine code instructions */
    vector<string> instructions;

    string line;
    while (getline(f, line)) {
        size_t pos = line.find("#");
        if (pos != string::npos)
            line = line.substr(0, pos);
        if (line.size() > 0)
            instructions.push_back(line);
    }

    instruction_parser(instructions);

    vector<Label> labels;
    vector<string> string_rgs;
    vector<int> int_rgs;


    for (size_t i = 0; i < instructions.size(); i++) {
        if (label_delimiter(instructions[i], i).valid)
            labels.push_back(label_delimiter(instructions[i], i));
    }



    int program_counter = 0;


    for (size_t i = 0; i < instructions.size(); i++) {
        int_rgs = three_comma_delimiter(address_delimiter(instructions[i]), labels);
        //cout << "opcode: " << opcode_delimiter(instructions[i]) << " " << "instructions: " << address_delimiter(instructions[i]) << " ";
        for (size_t i = 0; i < int_rgs.size(); i++) {
            //cout << int_rgs[i] << " ";
        }
        //cout << endl;
        string opcode = opcode_delimiter(instructions[i]);

        if (opcode.length() > 0) {
            bitset<16> instruction_in_binary(full_address_parser(opcode, program_counter, int_rgs));
            cout << "ram[" << program_counter << "] = 16'b" << instruction_in_binary << ";" << endl;
            program_counter++;
        }
    }

    return 0;
}

//ra0Eequ6ucie6Jei0koh6phishohm9

void remove_whitespace(string& a) {
    size_t begin = a.find_first_not_of(" \n\r\t\f\v");
    a = (begin == string::npos) ? "" : a.substr(begin);
}

void instruction_parser(vector<string>& instructions) {

    for (string& a : instructions) {
        remove_whitespace(a);
    }

    for (size_t i = 0; i < instructions.size(); i++) {
        string temp;
        string a = instructions[i];
        if (a.find(':') != string::npos) {
            temp = a.substr(a.find(':') + 1);
            if (temp == "" && (i < instructions.size() - 1)) {
                temp = a + ' ' + instructions[i + 1];
                instructions[i] = temp;
                instructions.erase(instructions.begin() + (i + 1));
            }
        }
    }

}//g++ -Wall sim.cpp -o sim