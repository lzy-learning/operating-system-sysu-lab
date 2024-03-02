CXX_COMPILER = g++
C_COMPILER = gcc
ASM_ASSEMBLER = nasm
ALL:main.out

main.out:main.o c_func.o cpp_func.o asm_func.o
	$(CXX_COMPILER) -o $@ $^ -m32

main.o:main.cpp
	$(CXX_COMPILER) -o $@ -m32 -c $<

c_func.o:c_func.c
	$(C_COMPILER) -o $@ -m32 -c $<

cpp_func.o:cpp_func.cpp
	$(CXX_COMPILER) -o $@ -m32 -c $<

asm_func.o:asm_func.asm
	$(ASM_ASSEMBLER) -o $@ -f elf32 $<

clean:
	rm -rf *.o *.out *.obj

run:
	./main.out
