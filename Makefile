./bin/bitonic32_32.o: ./src/bitonic.c
	mpcc ./src/bitonic.c -I/usr/include/sys -L/usr/lpp/ppe.hpct/lib -lhpc -lpmapi -o ./bin/bitonic32_32.o -lm -I/usr/lpp/ppe.hpct/include -L/usr/lpp/ppe.hpct/lib -lhpc -lpmapi
run: ./bitonic32_32.o
	./bitonic32_32.o ./input/input32.txt -procs 32
.PHONY: clean
clean:rm -r ./bin/bitonic32_32.o
