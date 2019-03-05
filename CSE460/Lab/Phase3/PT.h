#ifndef PT_H
#define PT_H

#include <vector>

using namespace std;

// forward declarations
class OS;
class VirtualMachine;

class Row
{
    int frame;
    bool valid;

friend
    class PT;

friend
    class OS;
};

class PT
{
    vector<Row> page_table;
    int psize;
    // for accounting info
    int num_ref; // # references
    int num_pf;  // # page faults

public:
    PT(int prog_size = 18, int ps = 8) // largest program subVector has 18 pages, each page is 8 words
    {
        page_table = vector<Row>(prog_size);
        psize = ps;
        num_ref = 0;
        num_pf = 0;
    }

    int log_to_phys(int addr)
    {
        num_ref++;

        int page_num = addr/psize;
        if (page_table[page_num].valid) {
            int frame = page_table[page_num].frame;
            int phys_addr = frame*psize + addr%psize;
            return phys_addr;
        } else { 
            // page fault
            num_pf++;
            return -1;
        }
    }

friend
    class OS;

friend
    class VirtualMachine;
};

#endif
