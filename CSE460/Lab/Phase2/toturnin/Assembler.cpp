/*****************************************************************************
Dustin Badillo, Bryan Soriano
Jan 28, 2019
Assembler.cpp
PHASE1
This file is the Assembler file that converts the assembly lines of code from
the .s file to an integer that will later be computed inside our VirtualMachine.
The first line in the assembly code will be read starting with the operator code
which will go into the if and find the proper operator code. After the opcode is
read rest of the is fed through and stored in the instruction. This file will
give an integer representing the assembly code
******************************************************************************/

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
	int rd = 0,
		rs = 0,
		addr = 0,
		constant = 0;
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
		/*when reading the stream the first line it comes into contact with
		 is the opcode followed by the rd then the addr*/
			str >> rd >> addr;
			if (rd < 0 || rd > 3) //checks if going outside bounds of register r[0]-r[3]
				{printf("Error #%d\n",0); return error;}
			if (addr < 0 || addr > 255) //checks if going outside of bounds of mem address 0 - 255
				{printf("Error #%d\n",1); return error;}
			instruction = 0; //instruction number
			instruction = instruction | rd << 9 | addr; /*places the registry
			directory and address in the instruction*/

		}
		else if (opcode == "loadi") {
			str >> rd >> constant;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",2); return error;}
			if (constant < -128 || constant > 127)
				{printf("Error #%d\n",3); return error;}
			instruction = 0;
			instruction = instruction | rd << 9 | 1 << 8 | (0xff & constant);

		}
		else if (opcode == "store") {
			str >> rd >> addr;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",4); return error;}
			if (addr < 0 || addr > 255)
				{printf("Error #%d\n",5); return error;}
			instruction = 1;
			instruction = instruction << 11 | rd << 9 | 1 << 8 | addr;
		}
		else if (opcode == "add") {
			str >> rd >> rs;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",6); return error;}
			if (rs < 0 || rs > 3)
				{printf("Error #%d\n",7); return error;}
			instruction = 2;
			instruction = instruction << 11 | rd << 9 | rs << 6;
		}
		else if (opcode == "addi") {
			str >> rd >> constant;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",8); return error;}
			if (constant < -128 || constant > 127)
				{printf("Error #%d\n",9); return error;}
			instruction = 2;
			instruction = instruction << 11 | rd << 9 | 1 << 8 | (0xff & constant);
		}
		else if (opcode == "addc") {
			str >> rd >> rs;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",10); return error;}
			if (rs < 0 || rs > 3)
				{printf("Error #%d\n",11); return error;}
			instruction = 3;
			instruction = instruction << 11 | rd << 9 | rs << 6;
		}
		else if (opcode == "addci") {
			str >> rd >> constant;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",12); return error;}
			if (constant < -128 || constant > 127)
				{printf("Error #%d\n",13); return error;}
			instruction = 3;
			instruction = instruction << 11 | rd << 9 | 1 << 8 | (0xff & constant);
		}
		else if (opcode == "sub") {
			str >> rd >> rs;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",14); return error;}
			if (rs < 0 || rs < 3)
				{printf("Error #%d\n",15); return error;}
			instruction = 4;
			instruction = instruction << 11 | rd << 9 | rs << 6;
		}
		else if (opcode == "subi") {
			str >> rd >> constant;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",16); return error;}
			if (constant < -128 || constant > 127)
				{printf("Error #%d\n",17); return error;}
			instruction = 4;
			instruction = instruction << 11 | rd << 9 | 1 << 8 | (0xff & constant);
		}
		else if (opcode == "subc") {
			str >> rd >> rs;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",18); return error;}
			if (rs < 0 || rs > 3)
				{printf("Error #%d\n",19); return error;}
			instruction = 5;
			instruction = instruction << 11 | rd << 9 | rs << 6;
		}
		else if (opcode == "subci") {
			str >> rd >> constant;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",20); return error;}
			if (constant < -128 || constant > 127)
				{printf("Error #%d\n",21); return error;}
			instruction = 5;
			instruction = instruction << 11 | rd << 9 | 1 << 8 | (0xff & constant);
		}
		else if (opcode == "and") {
			str >> rd >> rs;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",22); return error;}
			if (rs < 0 || rs > 3)
				{printf("Error #%d\n",23); return error;}
			instruction = 6;
			instruction = instruction << 11 | rd << 9 | rs << 6;
		}
		else if (opcode == "andi") {
			str >> rd >> constant;
			if (rs < 0 || rd > 3)
				{printf("Error #%d\t%d\t%d\n",24,rs,rd); return error;}
			if (constant < -128 || constant > 127)
				{printf("Error #%d\n",25); return error;}
			instruction = 6;
			instruction = instruction << 11 | rd << 9 | 1 << 8 | (0xff & constant);
		}
		else if (opcode == "xor") {
			str >> rd >> rs;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",26); return error;}
			if (rs < 0 || rs > 3)
				{printf("Error #%d\n",27); return error;}
			instruction = 7;
			instruction = instruction << 11 | rd << 9 | rs << 6;
		}
		else if (opcode == "xori") {
			str >> rd >> constant;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",28); return error;}
			if (constant < -128 || constant > 127)
				{printf("Error #%d\n",29); return error;}
			instruction = 7;
			instruction = instruction << 11 | rd << 9 | (0xff & constant);
		}
		else if (opcode == "compl") {
			str >> rd;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",30); return error;}
			instruction = 8;
			instruction = instruction << 11 | rd << 9;
		}
		else if (opcode == "shl") {
			str >> rd;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",31); return error;}
			instruction = 9;
			instruction = instruction << 11 | rd << 9;
		}
		else if (opcode == "shla") {
			str >> rd;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",32); return error;}
			instruction = 10;
			instruction = instruction << 11 | rd << 9;
		}
		else if (opcode == "shr") {
			str >> rd;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",33); return error;}
			instruction = 11;
			instruction = instruction << 11 | rd << 9;
		}
		else if (opcode == "shra") {
			str >> rd;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",34); return error;}
			instruction = 12;
			instruction = instruction << 11 | rd << 9;
		}
		else if (opcode == "compr") {
			str >> rd >> rs;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",35); return error;}
			if (rs < 0 || rs > 3)
				{printf("Error #%d\n",36); return error;}
			instruction = 13;
			instruction = instruction << 11 | rd << 9 | rs << 6;
		}
		else if (opcode == "compri") {
			str >> rd >> constant;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",37); return error;}
			if (constant < -128 || constant > 127)
				{printf("Error #%d\n",38); return error;}
			instruction = 13;
			instruction = instruction << 11 | rd << 9 | 1 << 8 | (0xff & constant);
		}
		else if (opcode == "getstat") {
			str >> rd;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",39); return error;}
			instruction = 14;
			instruction = instruction << 11 | rd << 9;
		}
		else if (opcode == "putstat") {
			str >> rd;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",40); return error;}
			instruction = 15;
			instruction = instruction << 11 | rd << 9;
		}
		else if (opcode == "jump") {
			str >> addr;
			if (addr < 0 || addr > 255)
				{printf("Error #%d\n",41); return error;}
			instruction = 16;
			instruction = instruction << 11 | addr;
		}
		else if (opcode == "jumpl") {
			str >> addr;
			if (addr < 0 || addr > 255)
				{printf("Error #%d\n",42); return error;}
			instruction = 17;
			instruction = instruction << 11 | addr;
		}
		else if (opcode == "jumpe") {
			str >> addr;
			if (addr < 0 || addr > 255)
				{printf("Error #%d\n",43); return error;}
			instruction = 18;
			instruction = instruction << 11 | addr;
		}
		else if (opcode == "jumpg") {
			str >> addr;
			if (addr < 0 || addr > 255)
				{printf("Error #%d\n",44); return error;}
			instruction = 19;
			instruction = instruction << 11 | addr;
		}
		else if (opcode == "call") {
			str >> addr;
			if (addr < 0 || addr > 255)
				{printf("Error #%d\n",45); return error;}
			instruction = 20;
			instruction = instruction << 11 | addr;
		}
		else if (opcode == "return") {
			str >> rd;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",46); return error;}
			instruction = 21;
			instruction = instruction << 11 | rd << 9;
		}
		else if (opcode == "read") {
			str >> rd;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",47); return error;}
			instruction = 22;
			instruction = instruction << 11 | rd << 9;
		}
		else if (opcode == "write") {
			str >> rd;
			if (rd < 0 || rd > 3)
				{printf("Error #%d\n",48); return error;}
			instruction = 23;
			instruction = instruction << 11 | rd << 9;
		}
		else if (opcode == "halt") {
			instruction = 24;
			instruction = instruction << 11;
		}
		else if (opcode == "noop") {
			instruction = 25;
			instruction = instruction << 11;
		}
		else
			{printf("Error #%d\n",49); return error;}
		out << setfill('0') << setw(5) << instruction << endl;
		getline(in, line);
	}
	return success;
} // assemble

