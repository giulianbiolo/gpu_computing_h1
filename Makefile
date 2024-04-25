CC = g++
CFLAGS = -g3

.PHONY: transpose_single
.PHONY: transpose_all

transpose_single: transpose_single.cpp
	$(CC) $(CFLAGS) transpose_single.cpp -o transpose_single $(OPTLEVEL)

transpose_all: transpose_all.cpp
	$(CC) $(CFLAGS) transpose_all.cpp -o transpose_all $(OPTLEVEL)

run_single: transpose_single
	./transpose_single $(N) $(B)

run_all: transpose_all
	./transpose_all $(N) $(B)

valgrind_single: transpose_single
	valgrind --tool=cachegrind --cache-sim=yes ./transpose_single $(N) $(B)
	make clean_valgrind

valgrind_all: transpose_all
	valgrind --tool=cachegrind --cache-sim=yes ./transpose_all
	make clean_valgrind

clean:
	rm -f transpose_single transpose_all cachegrind.out.*

clean_valgrind:
	rm -f cachegrind.out.*
