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

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>

#include "Assembler.h"
#include "VirtualMachine.h"
#include "OS.h"

using namespace std;

OS::~OS() { //write accounting
list<PCB *>::iterator ptr;// creating an iterator

    int cpu_time = 0;
    for (ptr = jobs.begin(); ptr != jobs.end(); ptr++)//incremanting cpu time for every jon untill complete
        cpu_time += (*ptr)->cpu_time;

    for (ptr = jobs.begin(); ptr != jobs.end(); ptr++) {//updating all information for ever job.
        (*ptr)->out << "Turn around time = " << (*ptr)->turn_around_time << ", CPU time = " << (*ptr)->cpu_time
              << ", Wait time = " << (*ptr)->wait_time << ", IO time = " << (*ptr)->io_time << endl;

        (*ptr)->out << "Total CPU time = " << cpu_time << ", System time = " << sys_time
              << ", Idle time = " << idle_time << ", Final clock = " << vm.clock << endl
              << "Real CPU Utilization = " << setprecision(3) /*sets decimal precision for float*/ 
	      << (float) cpu_time / vm.clock * 100 << "%, "
              << "System CPU Utilization = " << (float) (vm.clock - idle_time) / vm.clock * 100 << "%, "
              << "Throughput = " << jobs.size() / ((float) vm.clock / 1000) << endl << endl;

        (*ptr)->in.close();
        (*ptr)->out.close();
        (*ptr)->stack.close();
    }
    progs.close();

}

void OS::run() //(2)run OS 
{
	//the first process to be running is from the front of readyQ
	running = readyQ.front();
	//remove item from the readyQ that is running
	readyQ.pop();

	//keep looping while there is a process in running
	while (running != 0) {
		//set vm clock
		vm.clock = vm.clock + CONTEXT_SWITCH_TIME;
		//set system time
		sys_time = sys_time + CONTEXT_SWITCH_TIME;
		//load the process to the vm
		loadState();
		//run the process in the vm
		vm.run(TIME_SLICE, running->in, running->out);
		//set cpu time
		running->cpu_time = running->cpu_time + vm.clock;
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
	for (int i = vm.sp; i < 256; i++) {
		running->stack >> vm.mem[i]; //grabs from the stack and places in memory
	}
}

void OS::saveState() //(3)save vm => running
{
	//running is a pointer to the PCB objects
	//placing items from the vm back to the pcb
	running->pc = vm.pc;
	running->sr = vm.sr;
	running->sp = vm.sp;
	running->base = vm.base;
	running->ir = vm.ir;
	running->limit = vm.limit;
	running->r[0] = vm.r[0];
	running->r[1] = vm.r[1];
	running->r[2] = vm.r[2];
	running->r[3] = vm.r[3];
	running->stack.seekp(0, ios::beg); //seekp set position of output sequence
	for (int i = vm.sp; i < 256; i++) {
		running->stack << vm.mem[i]; //<<endl; //outputs from memory to the stack
	}

}

void OS::contextSwitch() // (4) switch(vm.ret)
{
	while (!waitQ.empty() && waitQ.front()->io_completion <= vm.clock) {
        readyQ.push(waitQ.front());
        waitQ.front()->wait_time_begin = vm.clock;
        waitQ.front()->io_time += (vm.clock - waitQ.front()->io_time_begin);
        waitQ.pop();
    }

    int vm_status = (vm.sr >> 5) & 07;//not sure waht is does...
    switch (vm_status) {
        case 0: //Time slice
            readyQ.push(running);
            running->wait_time_begin = vm.clock;
            break;
        case 1: //Halt
            running->out << running->prog << ": Terminated\n";
            running->turn_around_time = vm.clock;
            break;
        case 2: //Out of Bound Error
            running->out << running->prog << ": Out of bound Error, pc = " << vm.pc << endl;
            running->turn_around_time = vm.clock;
            break;
        case 3: //Stack Overflow
            running->out << running->prog << ": Stack overflow, pc = " << vm.pc << ", sp = " << vm.sp << endl;
            running->turn_around_time = vm.clock;
            break;
        case 4: //Stack Underflow
            running->out << running->prog << ": Stack underflow, pc = " << vm.pc << ", sp = " << vm.sp << endl;
            running->turn_around_time = vm.clock;
            break;
        case 5: //Bad Opcode
            running->out << running->prog << ": Bad opcode, pc = " << vm.pc << endl; 
            running->turn_around_time = vm.clock;
            break;
        case 6: //Read
        case 7: //Write
            waitQ.push(running);
            running->io_completion = vm.clock + 27;
            running->io_time_begin = vm.clock;
            break;
        default:
            cerr << running->prog << ": Unexpected status = " << vm_status 
                 << " pc = " << vm.pc << " time = " << vm.clock << endl;
            running->out << running->prog << ": Unexpected status: " << vm_status 
                 << " pc = " << vm.pc << " time = " << vm.clock << endl;
            running->turn_around_time = vm.clock;
    }

    saveState();
    running = 0;
    if (not readyQ.empty()) {
        running = readyQ.front();
        running->wait_time += (vm.clock - running->wait_time_begin);
        readyQ.pop();
    } 
    else if (not waitQ.empty()){
        running = waitQ.front();
        waitQ.pop();
        idle_time += (running->io_completion - vm.clock);
        vm.clock = running->io_completion;
        // assume all of context switch time is incurred after idle time
        running->io_time += (vm.clock - running->io_time_begin);
    }

}

OS::OS()
{
	system("ls *.s > progs");//reads files from directory
	progs.open("progs", ios::in); //allows for input
	
	int base = 0;
	int limit;

	string assemblyFile;
	while (progs >> assemblyFile) {//while there is a .s file going in
		fstream assembly, objectCode;
		int pos = assemblyFile.find("."); //looks for the "." for start of file
		string name = assemblyFile.substr(0, pos);//name of the file which ends before the "."

		assembly.open(assemblyFile.c_str(), ios::in);
		objectCode.open((name + ".o").c_str(), ios::out);//opens corresponding ".o" file.
		if (!assembly.is_open() or !objectCode.is_open()) {
			cout << "Couldn't open" << endl;
			//exit(3);
		}
		if (as.assemble(assembly, objectCode)) { //checks if opcode from assembly file is valid
			cout << "Assembler Error\n";
			assembly.close();
			objectCode.close();
			continue;
			//exit(4);
		}
		assembly.close();
		objectCode.close();
		objectCode.open((name+".o").c_str(), ios::in);
        	//need to check if it open
        	vm.load(objectCode, base, limit);
        	objectCode.close();


		PCB * job = new PCB(assemblyFile, base, limit - base);
        	job->in.open((name+".in").c_str(), ios::in);
        	job->out.open((name+".out").c_str(), ios::out);
        	job->stack.open((name+".st").c_str(), ios::in | ios::out);
		
		jobs.push_back(job);
		base = limit;
	}
	vm.total_limit = limit;
	list<PCB *>::iterator ptr;
	
	for ( ptr = jobs.begin(); ptr != jobs.end(); ptr++){
		readyQ.push(*ptr);//pushing next job on the list to readyQ.
	}
}
