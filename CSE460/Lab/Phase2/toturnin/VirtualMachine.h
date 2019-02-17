/*****************************************************************************
Dustin Badillo, Bryan Soriano
Jan 28, 2019
CSE 460
Kay Zemoudeh
VirtualMachine.h
PHASE1

In this file it contains the size of the memory as the integers being translated
by the VirtualMachine.cpp file.
******************************************************************************/
#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H

#include <fstream>
#include <vector>

using namespace std;

class VirtualMachine {
	int msize;
	int rsize;
	int pc, ir, sr, sp, clock;
	vector<int> mem;
	vector<int> r;
	int base, limit;
public:
	VirtualMachine() : msize(256), rsize(4), clock(0)
	{
		mem = vector<int>(msize);
		r = vector<int>(rsize);
	}
	friend class OS;
	void run(int, fstream&, fstream&);
	int get_clock();
}; // VirtualMachine

#endif
