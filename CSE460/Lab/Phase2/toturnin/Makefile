# note every second line must start with a Tab character

os: os_main.o os.o VirtualMachine.o Assembler.o
	g++ -o os os_main.o os.o VirtualMachine.o Assembler.o

os_main.o: os_main.cpp
	g++ -c os_main.cpp

os.o: os.cpp OS.h
	g++ -c os.cpp

VirtualMachine.o: VirtualMachine.cpp VirtualMachine.h
	g++ -c VirtualMachine.cpp

Assembler.o: Assembler.cpp Assembler.h
	g++ -c Assembler.cpp

clean:
	rm Assembler.o VirtualMachine.o os.o os_main.o os






