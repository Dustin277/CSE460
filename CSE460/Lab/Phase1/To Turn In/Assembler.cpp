#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>

#include "Assembler.h"

using namespace std;

Assembler::Assembler()
{
}

int Assembler::assemble(fstream& in, fstream& out)
{
    string line;
    string opcode;
    int rd, rs, addr, constant;
    int instruction;

    const int success = false;
    const int error = true;
    //const int debug = false;

    getline(in, line);
    while (!in.eof()) {
        if (line[0] == '!' or line == "") { // ignore comment or empty lines
            getline(in, line);
            continue;
        }

        istringstream str(line.c_str());
        str >> opcode;

        if (opcode == "load") { //instruction format 2
            str >> rd >> addr;
            if (rd < 0 || rd > 3)
                return error;
            if (addr < 0 || addr > 255)
                return error;
            instruction = 0;
            instruction = instruction | rd<<9 | addr;

        } else if (opcode == "loadi") {
            str >> rd >> constant;
            if (rd < 0 || rd > 3)
                return error;
            if (constant < -128 || constant > 127)
                return error;
            instruction = 0;
            instruction = instruction | rd<<9 | 1<<8 | (0xff & constant);

        } else if (opcode == "store") {
            str >> rd >> addr;
            if (rd < 0 || rd > 3)
                return error;
            if (addr < 0 || addr > 255)
                return error;
            instruction = 1;
            instruction = instruction<<11 | rd<<9 | 1<<8 | addr;

        } else if (opcode == "add") {
            str >> rd >> rs;
            if (rd < 0 || rd > 3)
                return error;
            if (rs < 0 || rs > 3)
                return error;
            instruction = 2;
            instruction = instruction<<11 | rd<<9 | rs<<6;
        } else if(opcode == "addi"){
	        str >> rd >> constant;
	        if (rd < 0 || rd > 3)
	            return error;
	        if (constant < -128 || constant > 127)
	            return error;
	        instruction = 2;
	        instruction = instruction<<11 | rd<<9 | 1<<8 | (0xff & constant);
	    }
        else if(opcode == "addc"){  //not done
            str >> rd >> rs;
            if (rd < 0 || rd > 3)
                return error;
            if (rs < 0 || rs > 3)
                return error;
            instruction = 3;
            instruction = instruction<<11 | rd<<9 | rs<<6;
        }
        else if(opcode == "addci"){ //not done
	        str >> rd >> constant;
	        if (rd < 0 || rd > 3)
	            return error;
	        if (constant < -128 || constant > 127)
	            return error;
	        instruction = 3;
	        instruction = instruction<<11 | rd<<9 | 1<<8 | (0xff & constant);
	    }
        else if(opcode == "sub"){
            str >> rd >> rs;
            if(rd < 0 || rd > 3)
                return error;
            if(rs < 0 || rs < 3)
                return error;
            instruction = 4
            instruction = instruction<<11 | rd<<9 | rs<<6; 
        }
        else if(opcode == "subi"){
            str >> rd >> constant;
            if(rd < 0 || rd > 3)
                return error;
            if(constant < -128 || constant > 127)
                return error;
            instruction = 4;
            instruction  = instruction<<11 | rd<<9 | 1<<8 | (0xff & constant);
        }
        else if(opcode == "subc"){
            str >> rd >> rs;
            if(rd < 0 || rd > 3)
                return error;
            if(rs < 0 || rs > 3)
                return error;
            instruction = 5;
            instruction = instruction<<11 | rd<<9 | rs<<6;
        }
        else if(opcode == "subci"){
            str >> rd >> constant;
            if (rd < 0 || rd > 3)
                return error;
            if (constant< -128 || constant > 127)
                return error;
            instruction = 5;
            instruction = instruction<<11 | rd<<9 | 1<<8 | (0xff & constant);
        }
        else if(opcode == "and"){
            str >> rd >> rs;
            if(rd < 0 || rd > 3)
                return error;
            if(rs < 0 || rs > 3)
                return error;
            instruction = 6;
            instruction = instruction<<11 | rd<<9 | rs<<6;
        }
        else if(opcode == "andi"){
            str >> rd >> constant;
            if (rs < 0 || rd > 3)
                return error;
            if (constant < -128 || constant > 127)
                return errorl
            instruction = 6;
            instruction = instruction<<11 | rd<<9 | 1<<8 | (0xff & constant);
        }
        else if(opcode == "xor"){
            str >> rd >> rs;
            if(rd < 0 || rd > 3)
                return error;
            if(rs < 0 || rs > 3)
                return error;
            instruction = 7;
            instruction = instruction<<11 | rd <<9 | rs<<6;
        }
        else if(opcode == "xori"){
            str >> rd >> constant;
            if (rd < 0 || rd > 3)
                return error;
            if (constant< -128 || constant > 127)
                return error;
            instruction = 7;
            instruction = instruction<<11 | rd<<9 | (0xff & constant);
        }
        else if(opcode == "compl"){
            str >> rd;
            if(rd < 0 || rd > 3)
                return error;
            instruction = 8;
            instruction = instruction<<11 | rd<<9;
        }
        else if(opcode == "shl"){
            str >> rd;
            if(rd < 0 || rd > 3)
                return error;
            instruction = 9;
            instruction = instruction<<11 | rd<<9;
        }
        else if(opcode == "shla"){
            str >> rd;
        }

	else if (opcode == "noop") {
            instruction = 25;
            instruction = instruction<<11;

        } else
            return error;

        out << setfill('0') << setw(5) << instruction << endl;
        getline(in, line);
    }
    return success;
} // assemble
