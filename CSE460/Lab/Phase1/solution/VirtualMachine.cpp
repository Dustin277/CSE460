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
        opcode = (ir&0xf800)>>11;
        rd = (ir&0x600)>>9;
        i = (ir&0x100)>>8;
        rs = (ir&0xc0)>>6;
        addr = ir&0xff;
        constant = addr;
    
        clock++;
    
        if (opcode == 0) { /* load loadi */
            if (i) {
                if (constant&0x80) constant |= 0xff00;
                r[rd] = constant;
            } else {
                if (addr >= limit) {
                    out << "Out of bound error.\n";
                    return;
                }
                r[rd] = mem[addr];
                clock += 3;
            }

        } else if (opcode == 1) { /* store */
            if (addr >= limit) {
                out << "Out of bound error.\n";
                return;
            }
            mem[addr] = r[rd];
            clock += 3;

        } else if (opcode == 2) { /* add addi */
            int sign1 = (r[rd]&0x8000)>>15;
            // sign extend
            if (sign1) r[rd] |= 0xffff0000;
            int sign2;
            if (i) {
                sign2 = (constant&0x80)>>7;
                // sign extend
                if (sign2) constant |= 0xffffff00;
                r[rd] = r[rd] + constant;
            } else {
                sign2 = (r[rs]&0x8000)>>15;
                int temp = r[rs];
                // sign extend
                if (sign2) temp |= 0xffff0000;
                r[rd] = r[rd] + temp;
            }

            // set CARRY
            if (r[rd]&0x10000) sr |= 0x1;
            else sr &= 0xfffe;

            // set OVERFLOW
            if (sign1 == sign2 and sign1 != (r[rd]&0x8000)>>15)
                sr |= 0x10;
            else
                sr &= 0xffef;

            // keep it at 16 bits
            r[rd] &= 0xffff;

        } else if (opcode == 3) { /* addc addci */
            int sign1 = (r[rd]&0x8000)>>15;
            // sign extend
            if (sign1) r[rd] |= 0xffff0000;
            int sign2;
            if (i) {
                sign2 = (constant&0x80)>>7;
                // sign extend
                if (sign2) constant |= 0xffffff00;
                r[rd] = r[rd] + constant + sr&0x1;
            } else {
                sign2 = (r[rs]&0x8000)>>15;
                int temp = r[rs];
                // sign extend
                if (sign2) temp |= 0xffff0000;
                r[rd] = r[rd] + temp + sr&0x1;
            }

            // set CARRY
            if (r[rd]&0x10000) sr |= 0x1;
            else sr &= 0xfffe;

            // set OVERFLOW
            if (sign1 == sign2 and sign1 != (r[rd]&0x8000)>>15)
                sr |= 0x10;
            else
                sr &= 0xffef;

            // keep it at 16 bits
            r[rd] &= 0xffff;

        } else if (opcode == 4) { /* sub subi */
            int sign1 = (r[rd]&0x8000)>>15;
            // sign extend
            if (sign1) r[rd] |= 0xffff0000;
            int sign2;
            if (i) {
                // sign extend
                sign2 = (constant&0x80)>>7;
                if (sign2) constant |= 0xffffff00;
                // negate and get sign
                constant *= -1;
                sign2 = (constant & 0x80000000)>>31;
                // now perform + instead of -
                r[rd] = r[rd] + constant;
            } else {
                // sign extend
                sign2 = (r[rs]&0x8000)>>15;
                int temp = r[rs];
                if (sign2) temp |= 0xffff0000;
                // negate and get sign
                temp *= -1;
                sign2 = (temp & 0x80000000)>>31;
                // now perform + instead of -
                r[rd] = r[rd] + temp;
            }

            // set CARRY
            if (r[rd]&0x10000) sr |= 0x1;
            else sr &= 0xfffe;

            // set OVERFLOW
            if (sign1 == sign2 and sign1 != (r[rd]&0x8000)>>15)
                sr |= 0x10;
            else
                sr &= 0xffef;

            // keep it at 16 bits
            r[rd] &= 0xffff;

        } else if (opcode == 5) { // subc subci OR subtract with borrow 
            int sign1 = (r[rd]&0x8000)>>15;
            // sign extend
            if (sign1) r[rd] |= 0xffff0000;
            int sign2;
            if (i) {
                // sign extend
                sign2 = (constant&0x80)>>7;
                if (sign2) constant |= 0xffffff00;
                // negate and get sign
                constant *= -1;
                sign2 = (constant & 0x80000000)>>31;
                // now perform + instead of -
                r[rd] = r[rd] + constant - sr&0x1;
            } else {
                // sign extend
                sign2 = (r[rs]&0x8000)>>15;
                int temp = r[rs];
                if (sign2) temp |= 0xffff0000;
                // negate and get sign
                temp *= -1;
                sign2 = (temp & 0x80000000)>>31;
                // now perform + instead of -
                r[rd] = r[rd] + temp - sr&0x1;
            }

            // set CARRY
            if (r[rd]&0x10000) sr |= 0x1;
            else sr &= 0xfffe;

            // set OVERFLOW
            if (sign1 == sign2 and sign1 != (r[rd]&0x8000)>>15)
                sr |= 0x10;
            else
                sr &= 0xffef;

            // keep it at 16 bits
            r[rd] &= 0xffff;

        } else if (opcode == 6) { /* and andi */
            if (i) r[rd] = r[rd] & constant;
            else r[rd] = r[rd] & r[rs];

        } else if (opcode == 7) { /* xor xori */
            if (i) r[rd] = r[rd] ^ constant;
            else r[rd] = r[rd] ^ r[rs];

        } else if (opcode == 8) { /* compl */
            r[rd] = ~r[rd];
            r[rd] &= 0xffff;

        } else if (opcode == 9) { /* shl */
            r[rd] <<= 1;
            if (r[rd]&0x10000) sr |= 0x1;
            else sr &= 0x1e;
            r[rd] &= 0xffff;

        } else if (opcode == 10) { /* shla */
            r[rd] <<= 1;
            if (r[rd]&0x10000) {
                sr |= 0x1;
                r[rd] |= 0x8000;
            } else {
                sr &= 0x1e;
                r[rd] &= 0x7fff;
            }
            r[rd] &= 0xffff;

        } else if (opcode == 11) { /* shr */
            if (r[rd]&0x1) sr |= 0x1;
            else sr &= 0xfffe;
            r[rd] >>= 1;

        } else if (opcode == 12) { /* shra */
            int sign = 0;
            if (r[rd]&0x8000) sign = 1;

            if (r[rd]&0x1) sr |= 0x1;
            else sr &= 0xfffe;

            r[rd] >>= 1;
            if (sign) r[rd] |= 0x8000;

        } else if (opcode == 13) { /* compr  compri */
            sr &= 0xfff1;
            if (i) {
                if (r[rd]<constant) sr |= 0x8;
                if (r[rd]==constant) sr |= 0x4;
                if (r[rd]>constant) sr |= 0x2;
            } else {
                if (r[rd]<r[rs]) sr |= 0x8;
                if (r[rd]==r[rs]) sr |= 0x4;
                if (r[rd]>r[rs]) sr |= 0x2;
            }

        } else if (opcode == 14) { /* getstat */
            r[rd] = sr;

        } else if (opcode == 15) { /* putstat */
            sr = r[rd];

        } else if (opcode == 16) { /* jump */
            if (addr >= limit) {
                out << "Out of bound error.\n";
                return;
            }

            pc = addr;

        } else if (opcode == 17) { /* jumpl */
            if (addr >= limit) {
                out << "Out of bound error.\n";
                return;
            }

            if (sr & 0x8) pc = addr;

        } else if (opcode == 18) { /* jumpe */
            if (addr >= limit) {
                out << "Out of bound error.\n";
                return;
            }

            if (sr & 0x4) pc = addr;

        } else if (opcode == 19) { /* jumpg */
            if (addr >= limit) {
                out << "Out of bound error.\n";
                return;
            }

            if (sr & 0x2) pc = addr;

        } else if (opcode == 20) { /* call */
            if (addr >= limit) {
                out << "Out of bound error.\n";
                return;
            }

            if (sp < limit+6) {
                cout << "Stack Overflow\n";
                exit(1); // stack overflow
            }

            mem[--sp] = pc;
            for (j=0; j<4; j++)
                mem[--sp] = r[j];
            mem[--sp] = sr;
            pc = addr;

            clock += 3;

        } else if (opcode == 21) { /* return */
            if (sp > 256-6) {
                cout << "Stack Underflow\n";
                exit(2); // stack underflow
            }

            sr = mem[sp++];
            for (j=3; j>=0; j--)
                r[j] = mem[sp++];
            pc = mem[sp++];

            clock += 3;

        } else if (opcode == 22) { /* read */
            in >> r[rd];
            // make sure value just read is within the range for 16 bits
            assert(r[rd] < 32768 and r[rd] >= -32768);
            r[rd] &= 0xffff;

            clock += 27;

        } else if (opcode == 23) { /* write */
            // sign extend for output
            int temp = r[rd];
            if (temp & 0x8000) temp |= 0xffff0000;
            out << temp << endl;

            clock += 27;

        } else if (opcode == 24) { /* halt */
            break;

        } else if (opcode == 25) { /* noop */
            /* do nothing */

        } else {
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
        for (j=0; j<limit; j++) {
            printf("%8d", mem[j]);
            if ((j%8)==7) printf("\n");
        }
        cout << endl;
    }
} /* main */
