/*****************************************************************************
Dustin Badillo, Bryan Soriano
Jan 28, 2019
CSE 460
Kay Zemoudeh 
VirtualMachine.cpp
PHASE1

This file it is getting the integer created by the assembler and executing the 
correct function stated in the assembly code. the begining of the while loop 
initalizes the sr, sp amd pc then checks the first part of the assembler integer 
for the opcode inside the nested ifs. 
******************************************************************************/
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstdlib>
#include <cassert>

#include "VirtualMachine.h"

using namespace std;

int VirtualMachine::get_clock()
{
	return clock;
}

void VirtualMachine::run(fstream& objectCode, fstream& in, fstream& out)
{
	const int debug = false;
	int opcode, rd, i, rs, constant, addr, j;

	base = 0;
	for (limit = 0; objectCode >> mem[limit]; limit++);

	sr = 2;
	sp = msize;
	pc = 0;
	while (pc < limit) {
		ir = mem[pc];
		pc++;
		opcode = (ir & 0xf800) >> 11;
		rd = (ir & 0x600) >> 9;
		i = (ir & 0x100) >> 8;
		rs = (ir & 0xc0) >> 6;
		addr = ir & 0xff;
		constant = addr;

		clock++;

		if (opcode == 0) { /* load loadi */
			if (i) {
				if (constant & 0x80) constant |= 0xff00;
				r[rd] = constant;
			}
			else {
				if (addr >= limit) {
					out << "Out of bound error.\n";
					return;
				}
				r[rd] = mem[addr];
				clock += 3;
			}

		}
		else if (opcode == 1) { /* store */
			if (addr >= limit) {
				out << "Out of bound error.\n";
				return;
			}
			mem[addr] = r[rd];
			clock += 3;

		}
		else if (opcode == 2) { /* add addi */
			int sign1 = (r[rd] & 0x8000) >> 15;
			int sign2;
			if (i) {
				sign2 = (constant & 0x80) >> 7;
				// sign extend both operands to perform operation
				if (sign2) constant |= 0xffffff00;
				if (sign1) r[rd] |= 0xffff0000;
				r[rd] = r[rd] + constant;
			}
			else {
				sign2 = (r[rs] & 0x8000) >> 15;
				int temp = r[rs];
				// sign extend both operands to perform operation
				if (sign2) temp |= 0xffff0000;
				if (sign1) r[rd] |= 0xffff0000;
				r[rd] = r[rd] + temp;
			}

			// set CARRY
			if (r[rd] & 0x10000) sr |= 0x1;
			else sr &= 0xfffe;

			// set OVERFLOW
			if (sign1 == sign2 and sign1 != (r[rd] & 0x8000) >> 15)
				sr |= 0x10;
			else
				sr &= 0xffef;

			// keep it at 16 bits
			r[rd] &= 0xffff;

		}
		else if (opcode == 3) { /* addc addci */
			int sign1 = (r[rd] & 0x8000) >> 15;
			int sign2;
			if (i) {
				sign2 = (constant & 0x80) >> 7;
				// sign extend both operands to perform operation
				if (sign2) constant |= 0xffffff00;
				if (sign1) r[rd] |= 0xffff0000;
				r[rd] = r[rd] + constant + sr & 0x1;
			}
			else {
				sign2 = (r[rs] & 0x8000) >> 15;
				int temp = r[rs];
				// sign extend both operands to perform operation
				if (sign2) temp |= 0xffff0000;
				if (sign1) r[rd] |= 0xffff0000;
				r[rd] = r[rd] + temp + sr & 0x1;
			}

			// set CARRY
			if (r[rd] & 0x10000) sr |= 0x1;
			else sr &= 0xfffe;

			// set OVERFLOW
			if (sign1 == sign2 and sign1 != (r[rd] & 0x8000) >> 15)
				sr |= 0x10;
			else
				sr &= 0xffef;

			// keep it at 16 bits
			r[rd] &= 0xffff;
		}
		else if (opcode == 4) { //sub subi
			int sign1 = (r[rd] & 0x8000) >> 15;
			int sign2;
			if (i) {
				sign2 = (constant & 0x80) >> 7;
				// sign extend both operands to perform operation
				if (sign2) constant |= 0xffffff00;
				if (sign1) r[rd] |= 0xffff0000;
				r[rd] = r[rd] - constant;
			}
			else {
				sign2 = (r[rs] & 0x8000) >> 15;
				int temp = r[rs];
				// sign extend both operands to perform operation
				if (sign2) temp |= 0xffff0000;
				if (sign1) r[rd] |= 0xffff0000;
				r[rd] = r[rd] - temp;
			}
		}
		else if (opcode == 5) { //subc subci
			int sign1 = (r[rd] & 0x8000) >> 15;
			int sign2;
			if (i) {
				sign2 = (constant & 0x80) >> 7;
				// sign extend both operands to perform operation
				if (sign2) constant |= 0xffffff00;
				if (sign1) r[rd] |= 0xffff0000;
				r[rd] = r[rd] - constant - sr & 0x1;
			}
			else {
				sign2 = (r[rs] & 0x8000) >> 15;
				int temp = r[rs];
				// sign extend both operands to perform operation
				if (sign2) temp |= 0xffff0000;
				if (sign1) r[rd] |= 0xffff0000;
				r[rd] = r[rd] - temp - sr & 0x1;
			}
		}
		else if (opcode == 6) { // and andi
			int sign1 = (r[rd] & 0x8000) >> 15;
			int sign2;
			if (i) {
				sign2 = (constant & 0x80) >> 7;
				if (sign2) constant |= 0xffffff00;
				if (sign1) r[rd] |= 0xffff0000;
				r[rd] &= r[rs];
			}
			else {
				sign2 = (r[rs] & 0x8000) >> 15;
				int temp = r[rs];
				if (sign2) temp |= 0xffff0000;
				if (sign1) r[rd] |= 0xffff0000;
				r[rd] &= temp;
			}

		}
		else if (opcode == 7) { //xor xori
			int sign1 = (r[rd] & 0x8000) >> 15;
			int sign2;
			if (i) {
				sign2 = (constant & 0x80) >> 7;
				if (sign2) constant |= 0xffffff00;
				if (sign1) r[rd] |= 0xffff0000;
				r[rd] ^= r[rs];
			}
			else {
				sign2 = (r[rs] & 0x8000) >> 15;
				int temp = r[rs];
				if (sign2) temp |= 0xffff0000;
				if (sign1) r[rd] |= 0xffff0000;
				r[rd] ^= temp;
			}
		}
		else if (opcode == 8) { //compl
			r[rd] = ~r[rd];
		}
		else if (opcode == 9) { //shl
			r[rd] = r[rd] << 1;
		}
		else if (opcode == 10) { //shla
			int sign1 = (r[rd] & 0x8000) >> 15;
			int temp = r[rd] << 1;
			if (sign1)
				r[rd] = temp | sign1 << 15;
			else
				r[rd] = temp & 0x7fff;
		}
		else if (opcode == 11) { //shr
			r[rd] = r[rd] >> 1;
		}
		else if (opcode == 12) { //shra
			int sign1 = (r[rd] & 0x8000) >> 15;
			int temp = r[rd] >> 1;
			if (sign1)
				r[rd] = temp | sign1 << 15;
			else
				r[rd] = temp & 0x7fff;
		}
		else if (opcode == 13) { //compr compi
			sr &= 0xffffffe1;
			if (i) {
				if (r[rd] < constant) sr |= 0x8;
				if (r[rd] == constant) sr |= 0x4;
				if (r[rd] > constant) sr |= 0x2;
			}
			else {
				if (r[rd] < r[rs]) sr |= 0x8;
				if (r[rd] == r[rs]) sr |= 04;
				if (r[rd] > r[rs]) sr |= 02;
			}

		}
		else if (opcode == 14) { //getstat
			r[rd] = sr;
		}
		else if (opcode == 15) { //putstat
			sr = r[rd];
		}
		else if (opcode == 16) { //jump
			pc = addr;
		}
		else if (opcode == 17) { //jumpl
			if (sr & 0x8 == 8)      //checks 4th bit on sr
				pc = addr;
		}
		else if (opcode == 18) { //jumpe
			if (sr & 0x4 == 4)      //checks 3rd bit on sr
				pc = addr;
		}
		else if (opcode == 19) { //jumpg
			if (sr & 0x2 == 2)      //checks 2nd bit on sr
				pc = addr;
		}
		else if (opcode == 20) {  //call (sp is decremented by 6 as the values of pc, r[0]-r[3], and sr in the VM are pushed on to stack)
			mem[--sp] = pc;     //first sp be decremented 
			for (j = 0; j < 4; j++)
				mem[--sp] = r[j];   //decrementing from r[0]-r[3]
			mem[--sp] = sr;         //dec for sr
			pc = addr;
			clock += 3;         //Each call instructions take 4 clock ticks to execute
		}
		else if (opcode == 21) { //return
			sr = mem[sp++];     //first sp be inccremented 
			for (j = 0; j < 4; j++)
				r[j] = mem[sp++];   //incrementing from r[0]-r[3]
			pc = mem[sp++];
			clock += 3;         //Each call instructions take 4 clock ticks to execute
		}
		else if (opcode == 22) {  //read
			in >> r[rd];        // reads from in using ">>" stream buffer
			clock += 27;
		}
		else if (opcode == 23) {  //write
			//sign ext 32 bits
			int sign1 = (r[rd] & 0x8000) >> 15;
			if (sign1) r[rd] |= 0xffff0000;
			
			out << r[rd] << endl;   //writes out from rd then n/
			clock += 27;
		}
		else if (opcode == 24) {  //halt
			return;
		}
		else if (opcode == 25) {  //noop
			return;
			//nothing
		}

		else {
			cout << "Bad opcode = " << opcode << endl;
			exit(3);
		}

		if (debug) {
			printf("ir=%d op=%d rd=%d i=%d rs=%d const=%d addr=%d\n", ir, opcode, rd, i, rs, constant, addr);
			printf("r[0]=%d r[1]=%d r[2]=%d r[3]=%d pc=%d sr=%d sp=%d clock=%d\n\n", r[0], r[1], r[2], r[3], pc, sr, sp, clock);
			//char c;
			//cin>>c;
		}
	}

	if (debug) {
		for (j = 0; j < limit; j++) {
			printf("%8d", mem[j]);
			if ((j % 8) == 7) printf("\n");
		}
		cout << endl;
	}
} /* main */
