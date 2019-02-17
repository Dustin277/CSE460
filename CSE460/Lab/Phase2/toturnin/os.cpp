/*****************************************************************************
Dustin Badillo, Bryan Soriano
Jan 28, 2019
CSE 460
Kay Zemoudeh 
os.cpp
PHASE2

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
#include "OS.h"

using namespace std;


OS::~OS(){ //write accounting

}

void OS::run() //(2)run OS 
{	
	//the first process to be running is from the front of readyQ
	running = readyQ.front();
	//remove item from the readyQ that is running
	readyQ.pop();
	
	//keep looping while there is a process running
	while(running!=0){
	    //set vm clock
	    vm.clock = vm.clock + CONTEXT_SWITCH_TIME;
	    //set system time
	    sys_time = sys_time + CONTEXT_SWITCH_TIME;
	    //load the process to the vm
	    loadState();
	    //run the process in the vm
	    vm.run(TIMESLICE, running->in,running->out);
	    //set cpu time
	    running->cpu_time = cpu_time + vm.clock;
	    //switch processes
	    contextSwitch();
	}
}

void OS::loadState() //(1)load running => vm 
{ 
	//running is a pointer to the PCB
	//placing items from the pcb to the vm
	vm.pc = running->pc;
	vm.ir = running->ir;
	vm.sr = running->sr;
	vm.base = running->base;
	vm.limit = running->limit;
	vm.sp = running->sp;
	vm.r[0] = running->r[0];
	vm.r[1] = running->r[1];
	vm.r[2] = running->r[2];
	vm.r[3] = running->r[3];
	running->stack.seekg(0, ios::beg); //seekg get position of input sequence
	//place everything in stack into vm memory size 256
	for (int i = vm.sp; i < 256; i++){
        	running->stack >> vm.mem[i];
	}
}

void OS::saveState() //(3)save vm => running
{
	//running is a pointer to the PCB objects
	//placing items from the vm back to the pcb
	running -> pc = vm.pc;
	running -> sr = vm.sr;
	running -> sp = vm.sp;
	running -> base = vm.base;
	running -> ir = vm.ir;
	running -> limit = vm.limit;
	running -> r[0]=vm.r[0];
	running -> r[1]=vm.r[1];
	running -> r[2]=vm.r[2];
	running -> r[3]=vm.r[3];
	running -> stack.seekp(0, ios::beg); //seekp set position of output sequence
	for(int i = vm.sp; i < 256 ; i++){
	running->stack << vm.mem[i]; //<<endl;
	}

}

void OS::contextSwitch() // (4) switch(vm.ret)
{

}
 
OS::OS(){ // assemble/ load / create PCB

	while(!in.eof()){
	//When the OS comes up it looks in current directory and gathers all *.s files: 
		system("ls *.s > progs") //prog is the string
		progs.open("progs", ios::in) //opens the file
	/*It then opens file progs and reads in file names. Each file is assembled,
	its object code loaded in memory, and a pointer to its PCB is stored in a linked-list:*/ 
		list<PCB *> jobs; //list of PCBs- jobs list
		PCB * p = new PCB;
		jobs.push_back(p);
			
	/* The processes (their PCBs) are either in ready, waiting,
	   or running state. Maintain two queue of processes, Ready Queue 
	   and Wait Queue, of type pointer to PCB:   */ 
		queue<PCB *> readyQ, waitQ;

	//  We also keep track of the running process by a pointer to its PCB:
		PCB * running;
	
	//pushback all processes in readyq
		
		
	} //end of while

}//end of OS

int main(int argc, char *argv[])
{

while(!in.eof()){
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
		
		}
		return 0;
	} // main


