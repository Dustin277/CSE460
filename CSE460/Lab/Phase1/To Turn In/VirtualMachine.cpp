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
		
		int sign1, sign2, temp;

		switch (opcode) {
		    case 0: /* load loadi */
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
			    break;
			case 1: /* store */
				if (addr >= limit) {
					out << "Out of bound error.\n";
					return;
				}
				mem[addr] = r[rd];
				clock += 3;
				break;
			case 2: /* add addi */
				sign1 = (r[rd] & 0x8000) >> 15;
				sign2;
				if (i) {
					sign2 = (constant & 0x80) >> 7;
					// sign extend both operands to perform operation
					if (sign2) constant |= 0xffffff00;
					if (sign1) r[rd] |= 0xffff0000;
					r[rd] = r[rd] + constant;
				}
				else {
					sign2 = (r[rs] & 0x8000) >> 15;
					temp = r[rs];
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
				break;
			case 3: /* addc addci */
				sign1 = (r[rd] & 0x8000) >> 15;
				sign2;
				if (i) {
					sign2 = (constant & 0x80) >> 7;
					// sign extend both operands to perform operation
					if (sign2) constant |= 0xffffff00;
					if (sign1) r[rd] |= 0xffff0000;
					r[rd] = r[rd] + constant + sr & 0x1;
				}
				else {
					sign2 = (r[rs] & 0x8000) >> 15;
					temp = r[rs];
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
				break;
			case 4: /* sub subi */
				sign1 = (r[rd] & 0x8000) >> 15;
				sign2;
				if (i) {
					sign2 = (constant & 0x80) >> 7;
					// sign extend both operands to perform operation
					if (sign2) constant |= 0xffffff00;
					if (sign1) r[rd] |= 0xffff0000;
					r[rd] = r[rd] - constant;
				}
				else {
					sign2 = (r[rs] & 0x8000) >> 15;
					temp = r[rs];
					// sign extend both operands to perform operation
					if (sign2) temp |= 0xffff0000;
					if (sign1) r[rd] |= 0xffff0000;
					r[rd] = r[rd] - temp;
				}
				break;
			case 5: /* subc subci */
				sign1 = (r[rd] & 0x8000) >> 15;
				sign2;
				if (i) {
					sign2 = (constant & 0x80) >> 7;
					// sign extend both operands to perform operation
					if (sign2) constant |= 0xffffff00;
					if (sign1) r[rd] |= 0xffff0000;
					r[rd] = r[rd] - constant - sr & 0x1;
				}
				else {
					sign2 = (r[rs] & 0x8000) >> 15;
					temp = r[rs];
					// sign extend both operands to perform operation
					if (sign2) temp |= 0xffff0000;
					if (sign1) r[rd] |= 0xffff0000;
					r[rd] = r[rd] - temp - sr & 0x1;
				}
				break;
			case 6: /* and andi */
				sign1 = (r[rd] & 0x8000) >> 15;
				sign2;
				if (i) {
					sign2 = (constant & 0x80) >> 7;
					if (sign2) constant |= 0xffffff00;
					if (sign1) r[rd] |= 0xffff0000;
					r[rd] &= r[rs];
				}
				else {
					sign2 = (r[rs] & 0x8000) >> 15;
					temp = r[rs];
					if (sign2) temp |= 0xffff0000;
					if (sign1) r[rd] |= 0xffff0000;
					r[rd] &= temp;
				}
				break;
			case 7: /* xor xori*/
				sign1 = (r[rd] & 0x8000) >> 15;
				sign2;
				if (i) {
					sign2 = (constant & 0x80) >> 7;
					if (sign2) constant |= 0xffffff00;
					if (sign1) r[rd] |= 0xffff0000;
					r[rd] ^= r[rs];
				}
				else {
					sign2 = (r[rs] & 0x8000) >> 15;
					temp = r[rs];
					if (sign2) temp |= 0xffff0000;
					if (sign1) r[rd] |= 0xffff0000;
					r[rd] ^= temp;
				}
				break;
			case 8: /* compl */
				r[rd] = ~r[rd];
				break;
			case 9: /*shl*/
				r[rd] = r[rd] << 1;
				break;
			case 10: /* shla */
				sign1 = (r[rd] & 0x8000) >> 15;
				temp = r[rd] << 1;
				if (sign1)
					r[rd] = temp | sign1 << 15;
				else
					r[rd] = temp & 0x7fff;
				break;
			case 11: /* shr */
				r[rd] = r[rd] >> 1;
				break;
			case 12: /* shra */
				sign1 = (r[rd] & 0x8000) >> 15;
				temp = r[rd] >> 1;
				if (sign1)
					r[rd] = temp | sign1 << 15;
				else
					r[rd] = temp & 0x7fff;
				break;
			case 13: //compr compi
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
				break;
			case 14: //getstat
				r[rd] = sr;
				break;
			case 15: //putstat
				sr = r[rd];
				break;
			case 16: //jump
				pc = addr;
				break;
			case 17: //jumpl
				if (sr & 0x8 == 8)      //checks 4th bit on sr
					pc = addr;
				break;
			case 18: //jumpe
				if (sr & 0x4 == 4)      //checks 3rd bit on sr
					pc = addr;
				break;
			case 19: //jumpg
				if (sr & 0x2 == 2)      //checks 2nd bit on sr
					pc = addr;
				break;
			case 20: //call (sp is decremented by 6 as the values of pc, r[0]-r[3], and sr in the VM are pushed on to stack)
				mem[--sp] = pc;     //first sp be decremented 
				for (j = 0; j < 4; j++)
					mem[--sp] = r[j];   //decrementing from r[0]-r[3]
				mem[--sp] = sr;         //dec for sr
				pc = addr;
				clock += 4;         //Each call instructions take 4 clock ticks to execute
				break;
			case 21: //return
				sr = mem[sp++];     //first sp be inccremented 
				for (j = 0; j < 4; j++)
					r[j] = mem[sp++];   //incrementing from r[0]-r[3]
				pc = mem[sp++];
				clock += 4;         //Each call instructions take 4 clock ticks to execute
				break;
			case 22: //read
				in >> r[rd];        // reads from in using ">>" stream buffer
				clock += 28;
				break;
			case 23: //write
				out << r[rd] << endl;   //writes out from rd then n/
				clock += 28;
				break;
			case 24: //halt
				return;
			case 25: //noop
				//nothing
				break;
			default: //opcode not recognized
				cout << "Bad opcode = " << opcode << endl;
				exit(3);
				break;
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
