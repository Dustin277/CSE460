# note every second line must start with a Tab character

os: os.o VirtualMachine.o Assembler.o
	g++ -o os os.o VirtualMachine.o Assembler.o

os.o: os.cpp
	g++ -c os.cpp

VirtualMachine.o: VirtualMachine.cpp VirtualMachine.h
	g++ -c VirtualMachine.cpp

Assembler.o: Assembler.cpp Assembler.h
	g++ -c Assembler.cpp

clean:
	rm Assembler.o VirtualMachine.o os.o os






