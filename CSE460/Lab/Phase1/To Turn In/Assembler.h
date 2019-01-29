/*****************************************************************************
Dustin Badillo, Bryan Soriano
Jan 28, 2019
Assembler.h
PHASE1

This file contains and implements the assembler class. 
******************************************************************************/


#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <fstream>

using namespace std;

class Assembler {
public:
    Assembler();
    int assemble(fstream& in, fstream& out); //reads the file
};

#endif
