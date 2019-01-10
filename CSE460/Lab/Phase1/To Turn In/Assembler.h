#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <fstream>

using namespace std;

class Assembler {
public:
    Assembler();
    int assemble(fstream& in, fstream& out);
};

#endif
