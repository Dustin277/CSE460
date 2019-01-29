/*****************************************************************************
Dustin Badillo, Bryan Soriano
Jan 28, 2019
CSE 460
Kay Zemoudeh 
os.cpp
PHASE1

This file is so far a rudimentary OS that so far and links the assembly files 
as well as the virtual machine files to get an OS enviroment that recieves 
assembly files, translates them to an integer that is read by the virtual machine
which in turn outputs the appropriate result as well as clock.
******************************************************************************/

#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>

#include "Assembler.h"
#include "VirtualMachine.h"

using namespace std;

int main(int argc, char *argv[])
{
	Assembler as; //calls the assembler
	VirtualMachine vm; //calls the virtual machine

	if (argc == 1) { //checks if there was a file given (must be .s)
		cout << "Must supply an assembly file name.\n";
		exit(1);
	}

	string assemblyFile = argv[1]; 
	int pos = assemblyFile.find("."); //looks for the "." for start of file
	//checks to make sure that the file is an assembly file
	if (pos > assemblyFile.length() || assemblyFile.substr(pos) != ".s") { 
		cout << "No .s suffix.\n";
		exit(2);
	}

	string name = assemblyFile.substr(0, pos); 
	string objectFile = name + ".o"; 

	fstream assembly, objectCode;
	assembly.open(assemblyFile.c_str(), ios::in);
	objectCode.open(objectFile.c_str(), ios::out);
	if (!assembly.is_open() or !objectCode.is_open()) {
		cout << "Couldn't open " << assemblyFile << " and/or " << objectFile << endl;
		exit(3);
	}

	if (as.assemble(assembly, objectCode)) { //checks if opcode from assembly file is valid
		cout << "Assembler Error\n";
		assembly.close();
		objectCode.close();
		exit(4);
	}

	assembly.close();
	objectCode.close();

	objectCode.open(objectFile.c_str(), ios::in);
	if (!objectCode.is_open()) {
		cout << "Couldn't open " << objectFile << endl;
		exit(5);
	}
	
	string inFile = name + ".in"; //input file for assembly code
	string outFile = name + ".out"; //output result
	fstream in, out;
	in.open(inFile.c_str(), ios::in);
	out.open(outFile.c_str(), ios::out);
	//check if either the in file or out file are not opening
	if (!in.is_open() or !out.is_open()) { 
		cout << "Couldn't open " << inFile << " and/or " << outFile << endl;
		exit(6);
	}

	vm.run(objectCode, in, out);
	out << "Clock = " << vm.get_clock() << endl; //outputs clock

	objectCode.close();
	in.close();
	out.close();

	return 0;
} // main
