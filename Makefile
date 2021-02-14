all: flush-tlb

flush-tlb: flush-tlb.c
	gcc -O0 -g -o flush-tlb flush-tlb.c -static -lrt -fno-strict-overflow

clean:
	rm -rf flush-tlb
