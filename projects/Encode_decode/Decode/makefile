GCC=gcc -g -Wall -Wextra -std=gnu11

decode: decode.o decode_lib.o ppm.o
	$(GCC) $^ -o $@ -lm
decode.o: decode.c
	$(GCC) $< -c
decode_lib.o: decode_lib.c decode_lib.h
	$(GCC) $< -c
ppm.o: ppm.c ppm.h
	$(GCC) $< -c
run: decode
	./decode
clean:
	rm -f *.o decode; clear
