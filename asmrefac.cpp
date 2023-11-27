#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

enum class Opcode : unsigned {
    ADD = 0, //000 rg1 rg2 rg3 0000
    SUB = 1, //000 rg1 rg2 rg3 0001
    AND = 2, //000 rg1 rg2 rg3 0010 
    OR = 3,  //000 rg1 rg2 rg3 0011 
    SLT = 4, //000 rg1 rg2 rg3 0100 
    JR = 8,  //000 rg1 000 000 1000 
    SLTI = 57344, //111 rg1 rg2 7b'imm
    LW = 32768,   //100 rg1 rg2 7b'imm
    SW = 40960, //101 rg1 rg2 7b'imm
    JEQ = 49152, //110 rg1 rg2 7b'rel_imm
    ADDI = 8192, //001 rg1 rg2 7b'imm
    J = 16384, //010 13b'imm
    JAL = 24576 //011 13b'imm
};

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
void instruction_parser(std::vector<std::string>&);

//takes in a string by reference
//removes extra whitespace from the string i.e. tabs // new lines
void remove_whitespace(std::string&);

//Label struct stores name and int value of label
struct Label {
    int value;
    std::string name;
    bool valid = false;
};
//takes in a string instruction after redundant whitespace has been removed
//if the instruction is not a label (calculated by detecting :) return what is before the first delimiter (space)
//else if there is a label then remove whitespace after the colon and extract the opcode from the leftovers
std::string opcode_delimiter(const std::string& instruction) {
    std::string opcode;
    if (instruction.find(':') == std::string::npos) {
        opcode = instruction.substr(0, instruction.find(' '));
    }
    else {
        std::string temp = instruction.substr(instruction.find(':') + 1);
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
std::string address_delimiter(const std::string& instruction) {
    std::string address;
    if (instruction.find(':') == std::string::npos) {
        address = instruction.substr(instruction.find(' ') + 1);
        remove_whitespace(address);
        address.erase(remove(address.begin(), address.end(), ' '), address.end());

    }
    else if (instruction.find(':') != std::string::npos) {
        std::string temp = instruction.substr(instruction.find(':') + 1);
        remove_whitespace(temp);
        if (temp.find('.') != std::string::npos) {
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
Label label_delimiter(const std::string& instruction, int index) {
    std::string label;
    Label labelStruct;
    if (instruction.find(':') != std::string::npos) {
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
bool containsNum(const std::string& stringPartition)
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

unsigned rgs_parser(const std::string& partition, const std::vector<Label>& labels) {


    for (Label b : labels) {
        if (partition == b.name)
            return b.value;
    }

    if (!containsNum(partition)) {
        return 0;
    }

    if (partition.find('$') != std::string::npos) {
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

std::vector<int> three_comma_delimiter(const std::string& address, const std::vector<Label>& labels) {
    std::vector<std::string> rgs;
    std::string temp = address;
    std::vector<int> int_rgs;
    std::string substring;

    for (size_t i = 0; i < 3; i++) {

        substring = temp.substr(0, temp.find(','));
        remove_whitespace(substring);
        rgs.push_back(substring);
        temp = temp.substr(temp.find(',') + 1);
    }

    if (rgs[1].find('(') != std::string::npos) {
        int_rgs.push_back(rgs_parser(rgs[0], labels));
        std::string memoryImm = rgs[1].substr(0, rgs[1].find('('));
        //cout << memoryImm;
        int_rgs.push_back(rgs_parser(memoryImm, labels));
        memoryImm = rgs[1].substr(rgs[1].find('(') + 1);
        std::string registerVal = memoryImm.substr(0, memoryImm.find(')'));
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

constexpr unsigned full_address_parser(const std::string& opcode, int index, const std::vector<int>& rgs, const std::vector<Label>& labels) {
    auto encode_three_reg_instr = [&](Opcode op) {
        return static_cast<unsigned>(op) | ((rgs[1] & 7) << 10) | ((rgs[2] & 7) << 7) | ((rgs[0] & 7) << 4);
    };

    auto encode_two_reg_imm_instr = [&](Opcode op) {
        int imm = (op == Opcode::LW) ? (rgs[2] & 127) : rgs[2];
        return static_cast<unsigned>(op) | ((rgs[0] & 7) << 10) | ((rgs[1] & 7) << 7) | imm;
        //return static_cast<unsigned>(op) | ((rgs[1] & 7) << 10) | ((rgs[0] & 7) << 7) | (rgs[2] & 127);
    };

    if (opcode == "add") return encode_three_reg_instr(Opcode::ADD);
    if (opcode == "sub") return encode_three_reg_instr(Opcode::SUB);
    if (opcode == "and") return encode_three_reg_instr(Opcode::AND);
    if (opcode == "or")  return encode_three_reg_instr(Opcode::OR);
    if (opcode == "slt") return encode_three_reg_instr(Opcode::SLT);
    if (opcode == "jr")  return static_cast<unsigned>(Opcode::JR) | ((rgs[0] & 7) << 10);
    if (opcode == "slti") return encode_two_reg_imm_instr(Opcode::SLTI);
    if (opcode == "lw")   return encode_two_reg_imm_instr(Opcode::LW);
    if (opcode == "sw")   return encode_two_reg_imm_instr(Opcode::SW);
    if (opcode == "jeq")  return static_cast<unsigned>(Opcode::JEQ) | ((rgs[0] & 7) << 10) | ((rgs[1] & 7) << 7) | ((rgs[2] - index - 1) & 127);
    if (opcode == "addi") return encode_two_reg_imm_instr(Opcode::ADDI);
    if (opcode == "j")    return static_cast<unsigned>(Opcode::J) | (rgs[0] & 8191);
    if (opcode == "jal")  return static_cast<unsigned>(Opcode::JAL) | (rgs[0] & 8191);

    // Pseudo instructions
    if (opcode == "movi") return static_cast<unsigned>(Opcode::ADDI) | ((0) << 10) | ((rgs[0] & 7) << 7) | (rgs[1] & 127);
    if (opcode == "nop")  return static_cast<unsigned>(Opcode::ADD) | (((0) << 10) | ((0) << 7) | ((0) << 4));
    if (opcode == "halt") return static_cast<unsigned>(Opcode::J) | (index);
    if (opcode == ".fill") return rgs[0];

    throw std::invalid_argument("Unknown opcode: " + opcode);
}

std::vector<std::string> assemblerOutput(int argc, char* argv[]) {
    /*
        Parse the command-line arguments
    */
    char* filename = nullptr;
    bool do_help = false;
    bool arg_error = false;
    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
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
        std::cerr << "usage " << argv[0] << " [-h] filename" << std::endl << std::endl;
        std::cerr << "Assemble E20 files into machine code" << std::endl << std::endl;
        std::cerr << "positional arguments:" << std::endl;
        std::cerr << "  filename    The file containing assembly language, typically with .s suffix" << std::endl << std::endl;
        std::cerr << "optional arguments:" << std::endl;
        std::cerr << "  -h, --help  show this help message and exit" << std::endl;
        return {};
    }

    /* iterate through the line in the file, construct a list
       of numeric values representing machine code */
    std::ifstream f(filename);
    if (!f.is_open()) {
        std::cerr << "Can't open file " << filename << std::endl;
        return {};
    }

    /* our final output is a list of ints values representing
       machine code instructions */
    std::vector<std::string> instructions;

    std::string line;
    while (getline(f, line)) {
        size_t pos = line.find("#");
        if (pos != std::string::npos)
            line = line.substr(0, pos);
        if (line.size() > 0)
            instructions.push_back(line);
    }

    instruction_parser(instructions);

    std::vector<Label> labels;
    std::vector<std::string> string_rgs;
    std::vector<int> int_rgs;


    for (size_t i = 0; i < instructions.size(); i++) {
        if (label_delimiter(instructions[i], i).valid)
            labels.push_back(label_delimiter(instructions[i], i));
    }



    int program_counter = 0;
    std::vector<std::string> result;

    for (size_t i = 0; i < instructions.size(); i++) {
        int_rgs = three_comma_delimiter(address_delimiter(instructions[i]), labels);
        //cout << "opcode: " << opcode_delimiter(instructions[i]) << " " << "instructions: " << address_delimiter(instructions[i]) << " ";
        // for (size_t i = 0; i < int_rgs.size(); i++) {
        //     //cout << int_rgs[i] << " ";
        // }
        //cout << endl;
        std::string opcode = opcode_delimiter(instructions[i]);

        if (opcode.length() > 0) {
            std::ostringstream oss;
            std::bitset<16> instruction_in_binary(full_address_parser(opcode, program_counter, int_rgs, labels));
            std::cout << "ram[" << program_counter << "] = 16'b" << instruction_in_binary << ";" << std::endl;
            oss << "ram[" << program_counter << "] = 16'b" << instruction_in_binary << ";";
            result.push_back(oss.str());
            program_counter++;
        }
    }
    return result;
}

/**
    Main function
    Takes command-line args as documented below
*/
int pmain(int argc, char* argv[]) {
    assemblerOutput(argc, argv);
    return 0;
}

void remove_whitespace(std::string& a) {
    size_t begin = a.find_first_not_of(" \n\r\t\f\v");
    a = (begin == std::string::npos) ? "" : a.substr(begin);
}

void instruction_parser(std::vector<std::string>& instructions) {

    for (auto& a : instructions) {
        remove_whitespace(a);
    }

    for (size_t i = 0; i < instructions.size(); i++) {
        std::string temp;
        std::string a = instructions[i];
        if (a.find(':') != std::string::npos) {
            temp = a.substr(a.find(':') + 1);
            if (temp == "" && (i < instructions.size() - 1)) {
                temp = a + ' ' + instructions[i + 1];
                instructions[i] = temp;
                instructions.erase(instructions.begin() + (i + 1));
            }
        }
    }

}

//g++ -std=c++20 -Isrc asm.cpp -o asm
//./asm fib_iter.s   
//time ./asm fib_iter.s              
//time ./asmold fib_iter.s