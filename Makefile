all:
	clang-format -i *.c
	gcc -g -F DWARF main.c -o main.bin
	gcc test.c -o test.bin
	./main.bin ./test.bin
	readelf -S ./test.bin
	./test.bin
clean:
	rm -vf *.bin
