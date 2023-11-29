#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <regex>
#include <vector>

using namespace std;

// size_t const static NUM_REGS = 8; 
// size_t const static MEM_SIZE = 1<<13;
// size_t const static REG_SIZE = 1<<16;


// size_t const static ADD = 0; //000 rg1 rg2 rg3 0000
// size_t const static SUB = 1; //000 rg1 rg2 rg3 0001
// size_t const static AND = 2; //000 rg1 rg2 rg3 0010 
// size_t const static OR = 3;  //000 rg1 rg2 rg3 0011 
// size_t const static SLT = 4; //000 rg1 rg2 rg3 0100 
// size_t const static JR = 8;  //000 rg1 000 000 1000 
// size_t const static SLTI = 57344; //111 rg1 rg2 7b'imm
// size_t const static LW = 32768;   //100 rg1 rg2 7b'imm
// size_t const static SW = 40960; //101 rg1 rg2 7b'imm
// size_t const static JEQ = 49152; //110 rg1 rg2 7b'rel_imm
// size_t const static ADDI = 8192; //001 rg1 rg2 7b'imm
// size_t const static J = 16384; //010 13b'imm
// size_t const static JAL = 24576; //011 13b'imm

// size_t const static THREE_REG_ARGUMENTS = 57359; //1110000000001111
// size_t const static TWO_REG_ARGUMENTS = 57344;   //1110000000000000

enum class Opcode : unsigned {
    NUM_REGS = 8,
    MEM_SIZE = 1<<13,
    REG_SIZE = 1<<16,
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
    JAL = 24576, //011 13b'imm
    THREE_REG_ARGUMENTS = 57359, //1110000000001111
    TWO_REG_ARGUMENTS = 57344 //1110000000000000
};

/*
    Loads an E20 machine code file into the list
    provided by mem. We assume that mem is
    large enough to hold the values in the machine
    code file.

    @param f Open file to read from
    @param mem Array represetnting memory into which to read program
*/
void load_machine_code(ifstream &f, unsigned mem[]) {
    regex machine_code_re("^ram\\[(\\d+)\\] = 16'b(\\d+);.*$");
    size_t expectedaddr = 0;
    string line;
    while (getline(f, line)) {
        smatch sm;
        if (!regex_match(line, sm, machine_code_re)) {
            cerr << "Can't parse line: " << line << endl;
            exit(1);
        }
        size_t addr = stoi(sm[1], nullptr, 10);
        unsigned instr = stoi(sm[2], nullptr, 2);
        if (addr != expectedaddr) {
            cerr << "Memory addresses encountered out of sequence: " << addr << endl;
            exit(1);
        }
        if (addr >= static_cast<unsigned>(Opcode::MEM_SIZE)) {
            cerr << "Program too big for memory" << endl;
            exit(1);
        }
        expectedaddr ++;
        mem[addr] = instr;
    }
}

void load_machine_code(const vector<string>& lines, unsigned mem[]) {
    regex machine_code_re("^ram\\[(\\d+)\\] = 16'b(\\d+);.*$");
    size_t expectedaddr = 0;

    for (const auto& line : lines) {
        smatch sm;
        if (!regex_match(line, sm, machine_code_re)) {
            cerr << "Can't parse line: " << line << endl;
            exit(1);
        }
        size_t addr = stoi(sm[1], nullptr, 10);
        unsigned instr = stoi(sm[2], nullptr, 2);
        if (addr != expectedaddr) {
            cerr << "Memory addresses encountered out of sequence: " << addr << endl;
            exit(1);
        }
        if (addr >= static_cast<unsigned>(Opcode::MEM_SIZE)) {
            cerr << "Program too big for memory" << endl;
            exit(1);
        }
        expectedaddr++;
        mem[addr] = instr;
    }
}

/*
    Prints the current state of the simulator, including
    the current program counter, the current register values,
    and the first memquantity elements of memory.

    @param pc The final value of the program counter
    @param regs Final value of all registers
    @param memory Final value of memory
    @param memquantity How many words of memory to dump
*/
string print_state(unsigned pc, unsigned regs[], unsigned memory[], size_t memquantity) {
    ostringstream oss;
    oss << setfill(' ');
    oss << "Final state:" << endl;
    oss << "\tpc=" <<setw(5)<< pc << endl;

    for (size_t reg=0; reg<static_cast<unsigned>(Opcode::NUM_REGS); reg++)
        oss << "\t$" << reg << "="<<setw(5)<<regs[reg] << endl;

    oss << setfill('0');
    bool cr = false;
    for (size_t count=0; count<memquantity; count++) {
        oss << hex << setw(4) << memory[count] << " ";
        cr = true;
        if (count % 8 == 7) {
            oss << endl;
            cr = false;
        }
    }
    if (cr)
        oss << endl;
    cout << oss.str();
    return oss.str();
}

ifstream filenameLoader(int argc, char* argv[], char*& filename){
    /*
        Parse the command-line arguments
    */
    bool do_help = false;
    bool arg_error = false;
    for (int i=1; i<argc; i++) {
        string arg(argv[i]);
        if (arg.rfind("-",0)==0) {
            if (arg== "-h" || arg == "--help")
                do_help = true;
            else
                arg_error = true;
        } else {
            if (filename == nullptr)
                filename = argv[i];
            else
                arg_error = true;
        }
    }
    /* Display error message if appropriate */
    if (arg_error || do_help || filename == nullptr) {
        cerr << "usage " << argv[0] << " [-h] filename" << endl << endl; 
        cerr << "Simulate E20 machine" << endl << endl;
        cerr << "positional arguments:" << endl;
        cerr << "  filename    The file containing machine code, typically with .bin suffix" << endl<<endl;
        cerr << "optional arguments:"<<endl;
        cerr << "  -h, --help  show this help message and exit"<<endl;
        return ifstream();
    }

    ifstream f(filename);
    if (!f.is_open()) {
        cerr << "Can't open file "<<filename<<endl;
        return ifstream();
    }
    return f;
}

int signExtend(int imm) {
    if ((imm >> 6) == 1) {
        return ((~imm & 127) + 1) * -1;
    }
    return imm;
}


string simulator(int argc, char *argv[]){
    char* filename = nullptr;
    ifstream f = filenameLoader(argc, argv, filename);
    if (!f) return "";

    //instantiating the blank variables
    unsigned mem[static_cast<unsigned>(Opcode::MEM_SIZE)] = {};
    unsigned rg[static_cast<unsigned>(Opcode::REG_SIZE)];
    unsigned pc = 0;

    //instructions vector the have the list of opcodes
    vector<unsigned> instructions;
    load_machine_code(f, mem);
    
    while(true) {
        //parses the machine code into digestable values 
        rg[0] = 0;
        unsigned val = mem[(pc & static_cast<unsigned>(Opcode::MEM_SIZE) - 1)];
        unsigned rg1 = (val & 7168) >> 10;
        unsigned rg2 = (val & 896) >> 7;
        unsigned rg3 = (val & 112) >> 4;
        int imm = signExtend(val & 127);
        unsigned imm2 = (val & 8191);

        switch(val & static_cast<unsigned>(Opcode::THREE_REG_ARGUMENTS)) {
            case static_cast<unsigned>(Opcode::ADD):
                rg[rg3] = (rg[rg1] + rg[rg2]) & 65535;
                break;
            case static_cast<unsigned>(Opcode::SUB):
                rg[rg3] = (rg[rg1] - rg[rg2]) & 65535;
                break;
            case static_cast<unsigned>(Opcode::AND):
                rg[rg3] = (rg[rg1] & rg[rg2]);
                break;
            case static_cast<unsigned>(Opcode::OR):
                rg[rg3] = (rg[rg1] | rg[rg2]);
                break;
            case static_cast<unsigned>(Opcode::SLT):
                rg[rg3] = (rg[rg1] < rg[rg2]) ? 1 : 0;
                break;
            case static_cast<unsigned>(Opcode::JR):
                if(pc == rg[rg1]){
                    pc = rg[rg1];
                    rg[0] = 0;
                    // print the final state of the simulator before ending, using print_state
                    return print_state(pc, rg, mem, 128);
                }
                pc = rg[rg1];
                break;
        }
        switch(val & static_cast<unsigned>(Opcode::TWO_REG_ARGUMENTS)) {
            case static_cast<unsigned>(Opcode::SLTI):
                rg[rg2] = (rg[rg1] < imm) ? 1 : 0;
                break;
            case static_cast<unsigned>(Opcode::LW):
                rg[rg2] = mem[rg[rg1] + imm];
                break;
            case static_cast<unsigned>(Opcode::SW):
                mem[rg[rg1] + imm] = rg[rg2];
                break;
            case static_cast<unsigned>(Opcode::JEQ):
                if(pc == ((rg[rg1] == rg[rg2]) ? pc + 1 + imm : pc + 1)){
                    pc = (rg[rg1] == rg[rg2]) ? pc + 1 + imm : pc + 1;
                    // print the final state of the simulator before ending, using print_state
                    rg[0] = 0;
                    return print_state(pc, rg, mem, 128);
                }
                pc = (rg[rg1] == rg[rg2]) ? pc + 1 + imm : pc + 1;
                break;
            case static_cast<unsigned>(Opcode::ADDI):
                rg[rg2] = (rg[rg1] + imm) & 65535;
                break;
            case static_cast<unsigned>(Opcode::J):
                if(pc == imm2){
                    pc = imm2;
                    // print the final state of the simulator before ending, using print_state
                    rg[0] = 0;
                    return print_state(pc, rg, mem, 128);
                }
                pc = imm2;
                break;
            case static_cast<unsigned>(Opcode::JAL):
                rg[7] = pc + 1;
                if(pc == imm2){
                    pc = imm2;
                    // print the final state of the simulator before ending, using print_state
                    rg[0] = 0;
                    return print_state(pc, rg, mem, 128);
                }
                pc = imm2;
                break;
        }  
        rg[0] = 0;
        pc++; //.fill
    }
    return "";
}

int main(int argc, char *argv[]){
    simulator(argc,argv);
}